#pragma once

#define FORWARDS(Name, Prefix) \
    class Prefix##Name; \
    using Name = std::shared_ptr<const Prefix##Name>
