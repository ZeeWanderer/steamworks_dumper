#include <iostream>
#include <fstream>
#include "clientmodule.h"
#include "enumdumper.h"
#include "clientinterfacedumper.h"
#include "callbackdumper.h"
#include "emsgdumper.h"

void DumpEnums(ClientModule* t_module, const std::string& t_outPath)
{
    EnumDumper dumper(t_module);
    size_t enumsFound = dumper.FindEnums();
    if(enumsFound)
    {
        std::cout << "Found " << enumsFound << " enums" << std::endl;

        size_t outPathSize = t_outPath.size() + 128;
        char* enumOutPath = new char[outPathSize];

        auto enums = dumper.GetEnums();
        for(auto it = enums->cbegin(); it != enums->cend(); ++it)
        {
            std::snprintf(enumOutPath, outPathSize, "%s/%s.h", t_outPath.c_str(), it->first.c_str());
            std::ofstream out(enumOutPath, std::ios_base::out);

            out << "typedef enum " << it->first << std::endl << "{" << std::endl;

            for(auto valIt = it->second.cbegin(); valIt != it->second.cend(); ++valIt)
            {
                out << "    k_" << it->first << valIt->second << " = " << valIt->first;
                out << "," << std::endl;
            }
            out << "} " << it->first << ";" << std::endl;
        }

        delete [] enumOutPath;
    }
}

void DumpInterfaces(ClientModule* t_module, const std::string& t_outPath)
{
    ClientInterfaceDumper iDumper(t_module);
    size_t numIfaces = iDumper.FindClientInterfaces();
    if(numIfaces)
    {
        std::cout << "Found " << numIfaces << " client interfaces" << std::endl;

        size_t outPathSize = t_outPath.size() + 128;
        char* fileOutPath = new char[outPathSize];

        auto vtables = iDumper.GetInterfaces();
        for(auto it = vtables->cbegin(); it != vtables->cend(); ++it)
        {
            std::snprintf(fileOutPath, outPathSize, "%s/%s.json", t_outPath.c_str(), it->first.c_str());
            std::ofstream out(fileOutPath, std::ios_base::out);

            out << "{" << std::endl;
            out << "    \"name\": \""                    << it->first            << "\"," << std::endl;
            out << "    \"found_at\": \"0x" << std::hex  << it->second.m_foundAt << std::dec << "\"," << std::endl;
            out << "    \"functions\": [ "  << std::endl;

            for(auto vtIt = it->second.m_functions.cbegin(); vtIt != it->second.m_functions.cend(); ++vtIt)
            {
                out << "        {" << std::endl;
                out << "            \"name\": \"" << vtIt->m_name << "\"," << std::endl;
                out << "            \"argc\": \"" << vtIt->m_argc << "\"," << std::endl;
                out << "            \"addr\": \"0x" << std::hex << vtIt->m_addr << std::dec << "\""  << std::endl;
                out << "        }";

                if(std::next(vtIt) != it->second.m_functions.cend())
                {
                    out << ",";
                }
                out << std::endl;
            }

            out << "    ]" << std::endl;
            out << "}" << std::endl;
        }
    }
}

void DumpCallbacks(ClientModule* t_module, const std::string& t_outPath)
{
    CallbackDumper cbDumper(t_module);
    size_t callbacksFound = cbDumper.FindCallbacks();
    if(callbacksFound)
    {
        std::cout << "Found " << callbacksFound << " callbacks" << std::endl;

        std::string outPath = t_outPath + "/callbacks.json";
        std::ofstream out(outPath, std::ios_base::out);

        out << "[" << std::endl;

        auto callbacks = cbDumper.GetCallbacks();
        for(auto it = callbacks->cbegin(); it != callbacks->cend(); ++it)
        {
            out << "    {" << std::endl;
            out << "        \"id\": "          << it->second.m_callbackID    << ","   << std::endl;
            out << "        \"name\": \""      << it->second.m_name          << "\"," << std::endl;
            out << "        \"size\": "        <<  it->second.m_callbackSize << ","   << std::endl;
            out << "        \"posted_at\": [";
            out << std::hex;
            for(auto pit = it->second.m_postedAt.cbegin(); pit != it->second.m_postedAt.cend(); ++pit)
            {
                out << "\"0x" << *pit << "\"";
                if(std::next(pit) != it->second.m_postedAt.cend())
                {
                    out << ",";
                }
            }
            out << std::dec;
            out << "]" << std::endl;
            out << "    }";

            if(std::next(it) != callbacks->cend())
            {
                out << ",";
            }

            out << std::endl;
        }
        out << "]" << std::endl;
    }
}

void DumpLegacyEMsgList(ClientModule* t_module, const std::string& t_outPath)
{
    EMsgDumper eDumper(t_module);
    size_t emsgsFound = eDumper.FindEMsgInfos();
    if(emsgsFound)
    {
        std::cout << "Found " << emsgsFound << " legacy EMsgs" << std::endl;

        std::string outPath = t_outPath + "/emsg_list.json";
        std::ofstream out(outPath, std::ios_base::out);

        out << "[" << std::endl;

        auto emsgList = eDumper.GetEMsgList();
        for(auto it = emsgList->cbegin(); it != emsgList->cend(); ++it)
        {
            out << "    {" << std::endl;
            out << "        \"emsg\": "        << it->first               <<  ","  << std::endl;
            out << "        \"flags\": "       << it->second.m_flags      <<  ","  << std::endl;
            out << "        \"server_type\": " << it->second.m_serverType <<  ","  << std::endl;
            out << "        \"name\": \""      << it->second.m_descriptor <<  "\"" << std::endl;
            out << "    }";

            if(std::next(it) != emsgList->cend())
            {
                out << ",";
            }

            out << std::endl;
        }

        out << "]" << std::endl;

    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage " << argv[0] << " <in_so> <out_path>" << std::endl;
        return -1;
    }

    const char* modulePath = argv[1];
    std::string outPath (argv[2]);

    std::cout << "Loading module image... ";
    ClientModule module(modulePath);
    if(!module.Load())
    {
        std::cout << "Could not load input file" << std::endl;
        return -1;
    }
    std::cout << "Done" << std::endl;

    std::cout << "Parsing image... ";
    if(!module.Parse())
    {
        std::cout << "Could not parse input file" << std::endl;
        return -1;
    }
    std::cout << "Done" << std::endl;

    DumpCallbacks(&module, outPath);
    DumpInterfaces(&module, outPath);
    DumpEnums(&module, outPath);
    DumpLegacyEMsgList(&module, outPath);

    return 0;
}
