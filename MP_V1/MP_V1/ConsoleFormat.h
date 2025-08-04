#pragma once

class ConsoleFormat {
public:
    virtual ~ConsoleFormat() = default;

    virtual void onEnabled() = 0;  
    virtual void process() = 0;   
};
