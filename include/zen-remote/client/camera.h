#pragma once

namespace zen::remote::client {

struct Camera {
  struct {
    float x0, y0, z0, w0, x1, y1, z1, w1, x2, y2, z2, w2, x3, y3, z3, w3;
  } vp;  // col major mat4
};

}  // namespace zen::remote::client
