#include <GL/freeglut.h>
#include <vector>

#include "lodepng/lodepng.h"
#include <iostream>
#include <format>
#include <functional>
#include <future>
#include <fstream>
#include <filesystem>
#include <type_traits>


namespace fs = std::filesystem;

class Player;
namespace {
    Player* g_player = nullptr;
    void playerDisplay();
}


struct Image {
    unsigned texture_id{ 0 };
    std::vector<unsigned char> raw_data{};
    uint32_t width{};
    uint32_t height{};
};

namespace Format {
    struct kPng {};
    struct kJpeg {};
    struct kMjpeg {};
} // namespace Format




class Player {
public:
    Player() {
        int tmp{};
        glutInit(&tmp, nullptr);
        glEnable(GLUT_DEBUG);
        glutInitDisplayMode(GLUT_SINGLE);
        glutInitWindowSize(300, 300);
        glutInitWindowPosition(100, 100);
        glutCreateWindow("Quern Player");

        g_player = this;
        glutDisplayFunc(playerDisplay);
    }

    ~Player() {
        if (task_.valid()) {
            std::cout << "Waiting for task to be closed...\n";
            task_.get();
        }
    }

    template<typename F>
    void Open(const std::vector<uint8_t>& data, F f) {
        std::cerr << "Not implemented\n";
    }

    template<>
    void Open<>(const std::vector<uint8_t>& data, Format::kPng) {
        unsigned error = lodepng::decode(img_.raw_data, img_.width, img_.height, data, LodePNGColorType::LCT_RGBA, 8);
        if (error) {
            std::cerr << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            throw std::runtime_error("Unable to decode data");
        }

        glGenTextures(1, &img_.texture_id);
        glBindTexture(GL_TEXTURE_2D, img_.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_.width, img_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            img_.raw_data.data());
    }

    template<>
    void Open<>(const std::vector<uint8_t>& data, Format::kMjpeg) {

    }

    void EnableDisplay() {
        //glut_task_ = std::async(std::launch::async, glutMainLoop);
    }

    void display()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, img_.texture_id);
        glBegin(GL_POLYGON);
        glTexCoord2f(-1.0, -1.0);
        glVertex2f(-1.0, 1.0);

        glTexCoord2f(1., -1.);
        glVertex2f(1., 1.);

        glTexCoord2f(1., 1.);
        glVertex2f(1., -1.);

        glTexCoord2f(-1., 1.);
        glVertex2f(-1., -1.);
        glEnd();
        glFlush();
    }

    std::future<void> task_;
    Image img_;
};

namespace {
    void playerDisplay() {
        if (g_player)
            g_player->display();
    }
}

std::vector<uint8_t> read_file(fs::path path) {
    auto f = std::ifstream{ path, std::ios::binary };
    if (!f) {
        std::cerr << "Cannot open file=" << path;
        return {};
    }

    auto start = std::istreambuf_iterator<char>{ f };
    auto end = decltype(start){};
    return { start , end };

}

/*class JpegDecoder {
public:
    JpegDecoder(const std::vector<uint8_t>& data) :
        data_(data)
    { }

    void decode() {
        auto marker_bytes = read(2);
        info_.ff_marker = marker_bytes[0] == 0xff && marker_bytes[1] == 0xd8;

        auto app = read(2);
        info_.app0 = app[0] << 8 | app[1];

        auto length = read(2);
        info_.length = length[0] << 8 | length[1];

        auto identifier = read(5);
        memcpy(info_.indentifier, identifier.data(), identifier.size());

        info_.ver_major = read(1).front();
        info_.ver_minor = read(1).front();

        info_.unit = read(1).front();

        auto x_density = read(2);
        info_.x_density = x_density[0] << 8 | x_density[1];
        auto y_density = read(2);
        info_.y_density = y_density[0] << 8 | y_density[1];

        auto x_thumbnail = read(2);
        info_.x_thumbnail = x_thumbnail[0] << 8 | x_thumbnail[1];
        auto y_thumbnail = read(2);
        info_.y_thumbnail = y_thumbnail[0] << 8 | y_thumbnail[1];

    }

    std::string GetInfo() {
        return std::format("INFO: ff_marker={} app0={} length={} id={} ver={}.{} unit={} dens={}x{} thumb={}x{}",
            info_.ff_marker, info_.app0, 
            info_.length, info_.indentifier, info_.ver_major, info_.ver_minor, info_.unit, info_.x_density, info_.y_density,
            info_.x_thumbnail, info_.y_thumbnail);
    }
private:
    std::vector<uint8_t> data_;

    std::vector<uint8_t> read(size_t bytes) {
        auto beg = data_.begin() + seeker_;
        seeker_ += bytes;
        if (seeker_ > data_.size()) {
            throw std::runtime_error("Seeker move outsides bands");
        }
        return { beg, beg + bytes };
    }

    size_t seeker_ = 0;

    struct Info {
        bool ff_marker{ false };
        int app0{};
        int length{};
        char indentifier[5]{};
        int ver_major{};
        int ver_minor{};
        int unit{};
        int x_density{};
        int y_density{};
        int x_thumbnail{};
        int y_thumbnail{};
    } info_;
};*/

int main(int argc, char** argv)
{
    auto p = std::make_unique<Player>();

    //auto filename = "big_bunny_thumbnail_vlc.png";
    //auto path = fs::path{ argv[0] }.parent_path() / filename;
    //auto encoded_picture = read_file(path);

     auto encoded_picture = read_file("D:/Downloads/Kaj.jpg");
    //  JpegDecoder decoder{ encoded_picture };
    //  decoder.decode();
    //  std::cout << decoder.GetInfo() << "\n";
    p->Open(encoded_picture, Format::kJpeg{});
    p->EnableDisplay();
    glutMainLoop();
    return 0;
}