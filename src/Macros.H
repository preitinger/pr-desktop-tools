#include <memory>

#define SCLASS(className) class className; typedef std::shared_ptr<className> className##SP; typedef std::weak_ptr<className> className##WP; class className

#define CB_CAST(type, var) type* var = (type*) data

#define ADD_TIMEOUT(step, type, var, method) Fl::add_timeout(step, [](void* data) { CB_CAST(type, var); var->method(); }, this)