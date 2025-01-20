#include "CommandReader.h"
#include "ApplicationController.h"
CommandReader::CommandReader(ApplicationController* app) :
    m_app(app)
{
  m_incomingByte = 0;
}
void CommandReader::loop() {
  m_incomingByte = m_app->readSerial(m_command,sizeof(m_command));
  if(m_incomingByte>0) {
    m_app->printf("Received[%d][%s]\r\n",m_incomingByte,m_command);
    m_app->executeCommand(m_command);
  }
}
