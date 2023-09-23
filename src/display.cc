#include "display.hh"

#include <thread>

#include "GL/freeglut_std.h"

DisplayOpenGL* DisplayOpenGL::g_self = nullptr;

DisplayOpenGL::DisplayOpenGL() {
  int tmp{};
  glutInit(&tmp, nullptr);
  glEnable(GLUT_DEBUG);
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Quern Player");

  // DisplayOpenGL::g_self = this;  // Meh, those c-apis...
  // auto cb = []() { DisplayOpenGL::g_self->display(); };
  // glutDisplayFunc(cb);
}

void DisplayOpenGL::AddToQueue(const Frame& frame) {
  // Maybe visitor? :)
  {
    auto lk = std::lock_guard{mx_display_texture_queue_};
    frames_.push_back(frame);
  }
}

void DisplayOpenGL::Run() {
  is_running_ = true;
  while (is_running_) {
    draw();
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0s);
  }
}
void DisplayOpenGL::Stop() { is_running_ = false; }

void DisplayOpenGL::draw() {
  Frame f;
  {
    auto lk = std::lock_guard{mx_display_texture_queue_};
    if (frames_.empty()) {
      return;
    }
    f = std::move(frames_.front());
    frames_.pop_front();
  }

  unsigned texture_id;
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, f.width, f.height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, f.raw_data.data());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture_id);
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