from requests import get  # to make GET request

def download_from_url(url, file_name):
    with open(file_name, "wb") as file:
        response = get(url)
        file.write(response.content)

url = 'http://localhost:8080/cpp/download.php?file=json-0.0.1514575489.676243933-macos_64-clang_9-debug-cmake.tar.gz'

print( download_from_url(url, "json-0.0.1514575489.676243933-macos_64-clang_9-debug-cmake.tar.gz") )

