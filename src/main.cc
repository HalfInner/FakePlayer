#include <GL/freeglut.h>
#include <vector>

#include "lodepng.h"
#include <iostream>
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

int main(int argc, char** argv)
{
    auto p = std::make_unique<Player>();

    auto filename = "big_bunny_thumbnail_vlc.png";
    auto path = fs::path{ argv[0] }.parent_path() / filename;
    auto encoded_picture = read_file(path);

    p->Open(encoded_picture, Format::kPng{});
    p->EnableDisplay();
    glutMainLoop();
    return 0;
}