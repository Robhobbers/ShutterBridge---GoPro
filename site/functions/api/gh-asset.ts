const ALLOWED_PREFIX =
  "https://github.com/Robhobbers/ShutterBridge---GoPro/releases/download/";

type PagesContext = {
  request: Request;
};

export const onRequestGet = async ({ request }: PagesContext) => {
  const url = new URL(request.url);
  const target = url.searchParams.get("url");

  if (!target || !target.startsWith(ALLOWED_PREFIX)) {
    return new Response("Invalid asset URL", { status: 400 });
  }

  const upstream = await fetch(target, {
    headers: { Accept: "application/octet-stream" },
    redirect: "follow",
  });

  if (!upstream.ok || !upstream.body) {
    return new Response(`Upstream error (${upstream.status})`, { status: 502 });
  }

  return new Response(upstream.body, {
    headers: {
      "Content-Type": "application/octet-stream",
      "Cache-Control": "public, max-age=3600",
      "X-Content-Type-Options": "nosniff",
    },
  });
};
