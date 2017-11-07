import SimpleHTTPServer
import SocketServer

class Handler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    pass

Handler.extensions_map['.wasm'] = 'application/wasm'

httpd = SocketServer.TCPServer(('', 3003), Handler)

httpd.serve_forever()
