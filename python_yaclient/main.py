from yaclient import YaClient
from server import Server

if __name__ == '__main__':
    server = Server()
    ya_client = YaClient(server)
    ya_client.run()
