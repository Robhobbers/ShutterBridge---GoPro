# Website deployment: ibexfuel.com

This fork deploys `site/` to Cloudflare Workers. The repository also contains
device firmware; that firmware is not the website build directory.

## Connect the fork

In Cloudflare Workers & Pages, create a Worker connected to GitHub with:

| Setting | Value |
| --- | --- |
| Repository | `Robhobbers/ShutterBridge---GoPro` |
| Production branch | `main` |
| Worker name | `ibexfuel` |
| Root directory | `site` |
| Build command | `pnpm build` |
| Deploy command | `pnpm run deploy` |
| Node version | 22.12.0 or newer (Node 22 LTS recommended) |
| pnpm version | 11.10.0, as pinned in `package.json` |

Cloudflare must install dependencies before the build (`pnpm install --frozen-lockfile`).
Set `NODE_VERSION` and `PNPM_VERSION` in the build settings if the build environment
does not pick the repository's versions automatically. This is a Workers deployment,
not a Pages drag-and-drop upload: language routing and `/api/gh-asset` need the Worker.

After the first deployment succeeds, verify its temporary `workers.dev` URL.
Then add `ibexfuel.com` as a Custom Domain under the Worker's Settings > Domains & Routes.
The domain must be active in the same Cloudflare account. Connect `www.ibexfuel.com`
there too if desired. Custom domains are managed in the dashboard, not hard-coded
into this repository, so preview builds cannot change domain routing accidentally.

Keep the domain registered with Hostinger. Before replacing its nameservers with
the pair assigned by Cloudflare, check that Cloudflare has all existing DNS records,
especially any email records. Do not cancel the domain registration.

## What belongs to this fork

- Canonical URLs, sitemap and robots.txt target `https://ibexfuel.com`.
- Source, support and firmware download links use `Robhobbers/ShutterBridge---GoPro`.
- The original author's attribution and license remain intact.
- The original author's YouTube account and analytics script are not used as this site's integrations.

The browser installer only lists published releases of this fork. Draft releases
are intentionally unavailable to visitors. An empty release list does not mean it
should fall back to the upstream project. Publish a tested release with the expected
`bootloader.bin`, `partitions.bin`, `boot_app0.bin`, `firmware.bin` and `littlefs.bin`
assets when ready; website deployment does not publish firmware releases.

## Local verification

From `site/`, install dependencies and run `pnpm build`, then `pnpm preview`.
Check the home page, localized documentation, `/flash`, sitemap and robots.txt.
Use a production hostname to check the release list: localhost intentionally uses
the development firmware manifest. Test invalid `/api/gh-asset` URLs return 400.

Once the Git integration is connected, pushes to `main` rebuild this fork's website.
Changes in the original repository do not deploy here unless deliberately brought
into this fork.
