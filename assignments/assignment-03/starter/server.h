// ===========================================================================
// server.h -- a tiny web server.  Written for you.  Do not change it.
//
// You do not need to read this file for the assignment.  It is here so that
// digits.cpp can show your digit reader in a browser.  If you are curious, it is a
// complete web server in about 200 lines: it opens a socket, waits for the
// browser to connect, reads one request, calls a function you give it, sends
// back the answer, and waits again.
//
//     serve(8080, [](const Request& req) {
//         Response res;
//         res.body = "hello";
//         return res;
//     });
//
// It only listens on 127.0.0.1, the computer you are sitting at: nobody else
// on the network can reach it.  It handles one request at a time.
//
// Works on Linux, macOS and WSL.  Native Windows is not supported: use WSL.
// ===========================================================================
#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
#error "server.h needs Linux, macOS or WSL. On Windows, build inside WSL."
#endif

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cctype>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>

struct Request {
    std::string method; // "GET" or "POST"
    std::string path; // "/classify"
    std::string query; // "k=3", still encoded
    std::string body; // the drawing, for POST

    // The value of one query parameter, decoded, or `fallback` if missing.
    std::string param(const std::string& name, const std::string& fallback = "") const {
        std::size_t i = 0;
        while (i <= query.size()) {
            std::size_t amp = query.find('&', i);
            if (amp == std::string::npos) amp = query.size();
            std::string pair = query.substr(i, amp - i);
            std::size_t eq = pair.find('=');
            if (eq != std::string::npos && pair.substr(0, eq) == name) {
                return decode(pair.substr(eq + 1));
            }
            i = amp + 1;
        }
        return fallback;
    }

    static std::string decode(const std::string& s) {
        std::string out;
        for (std::size_t i = 0; i < s.size(); i++) {
            if (s[i] == '+') {
                out += ' ';
            } else if (s[i] == '%' && i + 2 < s.size()) {
                out += static_cast<char>(std::strtol(s.substr(i + 1, 2).c_str(), nullptr, 16));
                i += 2;
            } else {
                out += s[i];
            }
        }
        return out;
    }
};

struct Response {
    int status = 200;
    std::string type = "application/json";
    std::string body;
};

namespace server_detail {

const std::size_t MAX_BODY = 64 * 1024 * 1024; // a drawing is about 3 KB

inline bool send_all(int fd, const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd, data.data() + sent, data.size() - sent, 0);
        if (n <= 0) return false;
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

inline const char* reason(int status) {
    switch (status) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 413: return "Payload Too Large";
        default: return "Internal Server Error";
    }
}

inline void reply(int fd, const Response& res) {
    std::string head = "HTTP/1.1 " + std::to_string(res.status) + " " + reason(res.status)
                     + "\r\nContent-Type: " + res.type
                     + "\r\nContent-Length: " + std::to_string(res.body.size())
                     + "\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n";
    if (send_all(fd, head)) send_all(fd, res.body);
}

// Reads one request.  Returns false if the browser hung up or sent nonsense.
inline bool read_request(int fd, Request& req, int& error) {
    std::string data;
    char buf[65536];
    std::size_t header_end;
    while ((header_end = data.find("\r\n\r\n")) == std::string::npos) {
        ssize_t n = ::recv(fd, buf, sizeof buf, 0);
        if (n <= 0 || data.size() > 65536) return false;
        data.append(buf, static_cast<std::size_t>(n));
    }
    std::string head = data.substr(0, header_end);
    std::size_t sp1 = head.find(' ');
    std::size_t sp2 = head.find(' ', sp1 + 1);
    if (sp1 == std::string::npos || sp2 == std::string::npos) return false;
    req.method = head.substr(0, sp1);
    std::string target = head.substr(sp1 + 1, sp2 - sp1 - 1);
    std::size_t q = target.find('?');
    req.path = target.substr(0, q);
    req.query = q == std::string::npos ? "" : target.substr(q + 1);

    std::size_t length = 0;
    std::string lower = head;
    for (char& c : lower) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    std::size_t at = lower.find("\r\ncontent-length:");
    if (at != std::string::npos) {
        length = std::strtoul(head.c_str() + at + 17, nullptr, 10);
    }
    if (length > MAX_BODY) {
        error = 413;
        return false;
    }
    req.body = data.substr(header_end + 4);
    while (req.body.size() < length) {
        ssize_t n = ::recv(fd, buf, sizeof buf, 0);
        if (n <= 0) return false;
        req.body.append(buf, static_cast<std::size_t>(n));
    }
    req.body.resize(length);
    return true;
}

} // namespace server_detail

// Serves forever.  Returns 1 only if the port cannot be opened.
template <typename Handler>
int serve(int port, Handler handle) {
    std::signal(SIGPIPE, SIG_IGN); // a closed browser tab must not kill us
    int listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        std::cerr << "cannot create a socket\n";
        return 1;
    }
    int yes = 1;
    ::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1 only
    addr.sin_port = htons(static_cast<unsigned short>(port));
    if (::bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof addr) < 0
        || ::listen(listener, 16) < 0) {
        std::cerr << "cannot use port " << port << ". Is another ./digits serve still "
                     "running? Stop it, or pick another port: ./digits serve 60000 8081\n";
        ::close(listener);
        return 1;
    }
    std::cout << "open http://localhost:" << port << " in your browser "
                 "(Ctrl-C here to stop)\n" << std::flush;

    while (true) {
        int fd = ::accept(listener, nullptr, nullptr);
        if (fd < 0) continue;
        Request req;
        Response res;
        int error = 400;
        if (!server_detail::read_request(fd, req, error)) {
            res.status = error;
            res.type = "text/plain";
            res.body = error == 413 ? "request too large" : "bad request";
        } else {
            try {
                res = handle(req);
            } catch (const std::exception& e) {
                res.status = 500;
                res.type = "text/plain";
                res.body = std::string("your code threw: ") + e.what();
            }
        }
        server_detail::reply(fd, res);
        ::close(fd);
    }
}

#endif
