const { test } = require('node:test');
const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const source = fs.readFileSync('firmware/data/app.js', 'utf8');
function fixture(fetch) {
  const events = [], timers = new Map(); let serial = 0;
  const nodes = new Proxy({}, { get: (o, k) => o[k] ||= {} });
  const c = vm.createContext({ fetch, AbortController, URLSearchParams,
    setTimeout: fn => { timers.set(++serial, fn); return serial; },
    clearTimeout: id => timers.delete(id),
    $: id => nodes[id], cfg: {}, boundMac: '', boundType: 0, wizType: 2,
    renderBound() {}, wizShow() {}, closeWizard: () => events.push('close'),
    setDirty: () => events.push('dirty'), toast: (message, error) => events.push({ message, error }),
    sleep: async () => {}, location: { reload: () => events.push('reload') },
  });
  vm.runInContext(source.slice(source.indexOf('let pairingSavePending'), source.indexOf('// ---- Live controls')), c);
  c.collectConfig = () => new URLSearchParams();
  const realReboot = c.rebootFlow;
  c.rebootFlow = async trigger => events.push(['reboot', trigger]);
  return { c, events, timers, realReboot };
}
test('pairing waits for storage acknowledgement; duplicate clicks do not save twice', async () => {
  let resolve, calls = 0;
  const f = fixture(() => { calls++; return new Promise(r => resolve = r); });
  const pending = f.c.wizBind('camera');
  await f.c.wizBind('other');
  assert.equal(calls, 1); assert.deepEqual(f.events, []);
  resolve({ ok: true, text: async () => 'reboot' }); await pending;
  assert.deepEqual(f.events, ['close', ['reboot', false]]);
  assert.equal(f.timers.size, 0);
});
test('failed persistence does not reboot and permits retry', async () => {
  const f = fixture(async () => ({ ok: false, status: 500 }));
  await f.c.wizBind('camera');
  assert.equal(f.events.some(x => Array.isArray(x)), false);
  assert.equal(f.events.at(-1).error, true);
  f.c.fetch = async () => ({ ok: true, text: async () => 'ok' });
  await f.c.wizBind('camera'); assert.equal(f.events.at(-1).message, 'Pairing saved');
});
test('lost acknowledgement times out without a second reboot', async () => {
  const f = fixture((url, opts) => new Promise((_, reject) =>
    opts.signal.addEventListener('abort', () => reject(new Error('timeout')))));
  const pending = f.c.wizBind('camera');
  for (const fn of f.timers.values()) fn();
  await pending;
  assert.equal(f.events.at(-1).error, true);
  assert.equal(f.events.some(x => Array.isArray(x)), false);
  assert.equal(f.timers.size, 0);
});
test('unexpected server reply is not treated as saved', async () => {
  const f = fixture(async () => ({ ok: true, text: async () => '<html>Error</html>' }));
  await f.c.wizBind('camera'); assert.equal(f.events.at(-1).error, true);
});
test('device-owned reboot does not call reboot endpoint; explicit reboot still does', async () => {
  const urls = [];
  const f = fixture(async url => { urls.push(url); return { ok: true }; });
  await f.realReboot(false); assert.deepEqual(urls, ['/status.json']);
  urls.length = 0; await f.realReboot(); assert.deepEqual(urls, ['/reboot', '/status.json']);
});
