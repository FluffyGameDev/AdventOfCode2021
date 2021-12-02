#include <functional>
#include <fstream>
#include <iostream>
#include <unordered_map>

struct CommandQuery
{
    std::string Name{};
    std::int32_t Argument{};
};

class ParsingContext
{
public:
    using Command = std::function<void(ParsingContext&, std::int32_t)>;

    void RegisterCommand(const std::string& commandName, Command command)
    {
        Commands[commandName] = command;
    }

    void ExecuteCommand(const CommandQuery& commandQuery)
    {
        Commands[commandQuery.Name](*this, commandQuery.Argument);
    }

    std::int64_t HorPosition{};
    std::int64_t Depth{};
    std::int64_t Aim{};

private:
    std::unordered_map<std::string, Command> Commands;
};

bool ParseInput(ParsingContext& context)
{
    static const char* inputFile{ "input.txt" };
    std::ifstream inputStream{ inputFile };

    bool readSucceeded{ inputStream.is_open() };
    if (readSucceeded)
    {
        CommandQuery commandQuery{};
        while (!inputStream.eof())
        {
            inputStream >> commandQuery.Name >> commandQuery.Argument;
            context.ExecuteCommand(commandQuery);
        }

        inputStream.close();
    }

    return readSucceeded;
}

namespace Commands
{
    namespace Part1
    {
        void RunForwardCommand(ParsingContext& context, std::int64_t argument)
        {
            context.HorPosition += argument;
        }

        void RunUpCommand(ParsingContext& context, std::int64_t argument)
        {
            context.Depth -= argument;
        }

        void RunDownCommand(ParsingContext& context, std::int64_t argument)
        {
            context.Depth += argument;
        }
    }

    namespace Part2
    {
        void RunForwardCommand(ParsingContext& context, std::int64_t argument)
        {
            context.HorPosition += argument;
            context.Depth += context.Aim * argument;
        }

        void RunUpCommand(ParsingContext& context, std::int64_t argument)
        {
            context.Aim -= argument;
        }

        void RunDownCommand(ParsingContext& context, std::int64_t argument)
        {
            context.Aim += argument;
        }
    }
}

int main()
{
    ParsingContext context{};

    namespace CommandRegistry = Commands::Part2; //Change to 'Part1' to solve part 1.
    context.RegisterCommand("forward", &CommandRegistry::RunForwardCommand);
    context.RegisterCommand("down", &CommandRegistry::RunDownCommand);
    context.RegisterCommand("up", &CommandRegistry::RunUpCommand);

	if (ParseInput(context))
	{
		std::cout << "HorPosition * Depth = " << context.HorPosition * context.Depth << '\n';
	}
	else
	{
		std::cout << "Failed to open input file.\n";
	}

	return 0;
}