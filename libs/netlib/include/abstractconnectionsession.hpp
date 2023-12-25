#pragma once

#include <memory>

class AbstractConnectionSession: public std::enable_shared_from_this<AbstractConnectionSession>
{
public:
    virtual ~AbstractConnectionSession() = default;

    virtual bool serverSide() const = 0;

protected:
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

};

