#include <fcntl.h>
#include <grpcpp/grpcpp.h>
#include <stdarg.h>
#include <unistd.h>

#include <algorithm>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/signals2/signal.hpp>
#include <functional>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>

#ifdef ZEN_REMOTE_USE_GRAPHICS_API_GLES_V32
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#endif

#ifdef ZEN_REMOTE_USE_GRAPHICS_API_OPENGL
#include <GL/glew.h>
#endif
