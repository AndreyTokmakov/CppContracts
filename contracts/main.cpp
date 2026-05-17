/**============================================================================
Name        : main.cpp
Created on  : 20.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines
============================================================================**/

#include <iostream>
#include <print>
#include <format>
#include <cmath>
#include <algorithm>

#include <string>
#include <string_view>
#include <vector>
#include <ranges>
#include <contracts>


void handle_contract_violation(const std::contracts::contract_violation& violation)
{
    std::println("Contract violation");
    std::println(std::cerr, "\tFile: {}\n\tLine: {}\n\tFunction: {}\n\tComment: {}",
        violation.location().file_name(),
        violation.location().line(),
        violation.location().function_name(),
        violation.comment());
    std::terminate();
}

namespace
{
    template<typename  T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
    {
        for (const auto& v : vec)
            os << v << " ";
        return os;
    }
}

namespace contracts_tests
{
    double calculateSquareRoot(const double value)
        pre(value >= 0)
    {
        return std::sqrt(value);
    }


    double divide(const int numerator, const int denominator)
        pre(denominator != 0)
    {
        return numerator / denominator;
    }

    std::vector<int> getTopValue(const std::vector<int>& values, const size_t size)
        pre(!values.empty())
        pre(size > 0 && size <= values.size())
    {
        std::cout << std::boolalpha << values.empty() << std::endl;

        std::vector<int> sorted = values;
        std::ranges::sort(sorted, std::greater<int>());
        return std::vector<int>{sorted.begin(), sorted.begin() + size};
    }

    void send(std::span<const std::byte> data, int fd)
        pre(fd >= 0)
        pre(!data.empty())
    {

    }

    std::string normalizeString(std::string text)
        post (r: r.empty())
        // post(!std::ranges::any_of(r: result, isspace))
    {
        std::erase_if(text, isspace);
        return text;
    }

    void test_Send()
    {
        send(std::vector<std::byte>{}, -1);

        /**
        Contract violation
            File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
            Line: 48
            Function: void contracts_tests::send(std::span<const std::byte>, int)
            Comment: fd >= 0
        terminate called without an active exception
        **/
    }

    void test_Divide()
    {
        std::cout << divide(10, 2) << std::endl;
        std::cout << divide(10, 0) << std::endl;

        /*
        5
        Contract violation
            File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
            Line: 107
            Function: double contracts_tests::divide(int, int)
            Comment: denominator != 0
        terminate called without an active exception
        */
    }

    void test_GetTopValue()
    {
        {
            const std::vector<int> values {1, 2, 3, 4, 5};
            const auto result = getTopValue(values, 3);
            std::cout << result << std::endl;
        }
        /*
        {
            std::vector<int> values {1, 2, 3, 4, 5};
            auto result = getTopValue(values, 32);
            std::cout << result << std::endl;
        } */
        {
            const std::vector<int> values;
            const auto result = getTopValue(values, 32);
            std::cout << result << std::endl;
        }

        /*
        false
        5 4 3
        Contract violation
            File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
            Line: 113
            Function: std::vector<int> contracts_tests::getTopValue(const std::vector<int>&, size_t)
            Comment: !values.empty()
        terminate called without an active exception

        */
    }
}



namespace Contracts::Class_Methods
{
    enum class ConnectionState
    {
        Idle,
        Disconnected,
        Connected,
        Error
    };

    template<typename  T>
    std::ostream& operator<<(std::ostream& stream, const ConnectionState state)
    {
        switch (state) {
            case ConnectionState::Idle: return stream << "Idle";
            case ConnectionState::Disconnected: return stream << "Disconnected";
            case ConnectionState::Connected:return stream << "Connected";
            case ConnectionState::Error: return stream << "Error";
        }
        return stream;
    }

    struct QueryResult {};

    struct DatabaseConnection
    {
        ConnectionState state { ConnectionState::Idle };
        std::string connectionString;

    public:

        DatabaseConnection() = default;

        explicit DatabaseConnection(std::string connectionString) :
            connectionString(std::move(connectionString)) {
        }

        void Connect()
            pre(state == ConnectionState::Disconnected || state == ConnectionState::Idle)
            pre(!connectionString.empty())
            post(state == ConnectionState::Connected || state == ConnectionState::Error)
        {
            try {
                state = ConnectionState::Connected;
                std::cout << "Connected to " << connectionString << std::endl;
            }
            catch (...) {
                state = ConnectionState::Error;
                std::cout << "Connection error" << std::endl;
            }
        }

        void Disconnect()
            pre(state == ConnectionState::Connected)
            post(state == ConnectionState::Disconnected)
        {
            state = ConnectionState::Disconnected;
            std::cout << "Disconnected" << std::endl;
        }

        QueryResult executeQuery(const std::string& query = std::string{}) const
            pre(state == ConnectionState::Connected || state == ConnectionState::Error)
            pre(!query.empty())
        {
            std::cout << "Executing query: " << query << std::endl;
            return QueryResult {};
        }
    };

    void Test()
    {
        {
            DatabaseConnection connection("localhost");
            connection.Connect();
            connection.Disconnect();

            // Connected to localhost
            // Disconnected

        }
        {
            DatabaseConnection connection;
            connection.Connect();
            connection.executeQuery("SELECT * FROM table");

            /*
            Contract violation
                File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
                Line: 195
                Function: void Contracts::Class_Methods::DatabaseConnection::Connect()
                Comment: !connectionString.empty()
            terminate called without an active exception
            */
        }
    }

    void Test2()
    {
        DatabaseConnection connection("localhost");
        connection.Disconnect();

        /*
        Contract violation
            File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
            Line: 209
            Function: void Contracts::Class_Methods::DatabaseConnection::Disconnect()
            Comment: state == ConnectionState::Connected
        terminate called without an active exception
        */
    }

    void Test3()
    {
        DatabaseConnection connection("localhost");
        connection.Connect();
        // connection.executeQuery("SELECT * FROM table");
        connection.executeQuery();
        connection.Disconnect();

        /*
        Connected to localhost
        Contract violation
            File: /home/andtokm/DiskS/ProjectsUbuntu/CppContracts/contracts/main.cpp
            Line: 218
            Function: Contracts::Class_Methods::QueryResult Contracts::Class_Methods::DatabaseConnection::executeQuery(const std::string&) const
            Comment: !query.empty()
        terminate called without an active exception
        */
    }
}

int main([[maybe_unused]] const int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // contracts_tests::test_Send();
    // contracts_tests::test_Divide();
    contracts_tests::test_GetTopValue();

    // Contracts::Class_Methods::Test();
    // Contracts::Class_Methods::Test2();
    // Contracts::Class_Methods::Test3();


    return EXIT_SUCCESS;
}

