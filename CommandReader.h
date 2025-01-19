#ifndef COMMANDREADER_H
#define COMMANDREADER_H

class ApplicationController;
class CommandReader
{
public:
    CommandReader(ApplicationController* app);
    void loop();
private:
    ApplicationController* m_app;
    int m_incomingByte;
    char m_command[256];
};

#endif // COMMANDREADER_H
