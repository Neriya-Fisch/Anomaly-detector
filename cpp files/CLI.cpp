#include "CLI.h"

CLI::CLI(DefaultIO *dio1)
{
    float p = 7;
    option = &p;
    dio = dio1;
    this->cliData = new CliData();
    commands[0] = new UploadCommand(dio, cliData);
    commands[1] = new ThresholdCommand(dio, cliData);
    commands[2] = new DetectCommand(dio, cliData);
    commands[3] = new DisplayCommand(dio, cliData);
    commands[4] = new AnalyzeCommand(dio, cliData);
    commands[5] = new ExitCommand(dio, cliData);
}

//activate the menu and execute every command
void CLI::start()
{
    while (*option != 6)
    {
        dio->write("Welcome to the Anomaly Detection Server.\n");
        dio->write("Please choose an option:\n");
        for (int i = 0; i < 6; i++)
        {
            string ss = commands[i]->getDescription();
            dio->write(ss);
        }
        dio->read(option);
        int x = *option - 1;
        commands[x]->execute();
    }
}

CLI::~CLI()
{
    for (int i = 0; i < 6; i++)
    {
        delete commands[i];
    }
    delete commands;
}
