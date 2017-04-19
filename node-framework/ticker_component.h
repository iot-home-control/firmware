#ifndef TICKER_COMPONENT_H
#define TICKER_COMPONENT_H

class ticker_component
{
public:
    virtual void update()=0;
    virtual ~ticker_component()=default;
};

#endif // TICKER_COMPONENT_H
