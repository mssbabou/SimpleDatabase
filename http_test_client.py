import socket
import sys

# Server configuration
HOST = 'localhost'
PORT = 80

def send_request(method, path, headers=None, body=None, chunked=False):
    try:
        # Create a socket and connect to the server
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))

            # Build the request
            request_line = f"{method} {path} HTTP/1.1\r\n"
            request_headers = ""
            if headers:
                for key, value in headers.items():
                    request_headers += f"{key}: {value}\r\n"

            # Send chunked body if specified
            if chunked and body:
                request_headers += "Transfer-Encoding: chunked\r\n\r\n"
                s.sendall(request_line.encode() + request_headers.encode())
                
                for chunk in body:
                    chunk_size = hex(len(chunk))[2:]
                    s.sendall(f"{chunk_size}\r\n".encode() + chunk.encode() + b"\r\n")
                s.sendall(b"0\r\n\r\n")
            else:
                if body:
                    request_headers += f"Content-Length: {len(body)}\r\n"
                request_headers += "\r\n"

                # Send the request
                s.sendall(request_line.encode() + request_headers.encode())
                if body:
                    s.sendall(body.encode())

            # Receive the response
            response = s.recv(4096).decode()
            print("Server response:")
            print(response)

    except ConnectionError as e:
        print(f"Connection error: {e}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    print("Select a request type:")
    print("1. GET")
    print("2. POST with Content-Length")
    print("3. POST with chunked encoding")

    choice = input("Enter your choice (1/2/3): ")

    if choice == "1":
        send_request("GET", "/")
    elif choice == "2":
        body = input("Enter POST body: ")
        send_request("POST", "/", headers={"Host": "localhost"}, body=body)
    elif choice == "3":
        chunks = []
        print("Enter chunks (type 'done' to finish):")
        while True:
            chunk = input()
            if chunk.lower() == "done":
                break
            chunks.append(chunk)
        send_request("POST", "/", headers={"Host": "localhost"}, body=chunks, chunked=True)
    else:
        print("Invalid choice.")
