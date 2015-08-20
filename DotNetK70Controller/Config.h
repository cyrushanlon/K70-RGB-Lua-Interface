#pragma once

#include<fstream>
#include<map>
#include <string>

//Config stores default script only at the moment

std::map<std::string, std::string> ConfigMap;

void SetupConfig()
{
	std::ifstream ConfigIn("config.txt");
	std::ofstream ConfigOut;

	//ConfigMap.clear();
	
	if (!ConfigIn.good()) // if config doesnt exist
	{
		ConfigIn.close();

		ConfigOut.open("config.txt");

		ConfigOut << "DefaultScript=rainbow;"; // relies on rainbow.lua existing
		ConfigMap.insert(std::make_pair("DefaultScript", ""));

		ConfigOut.close();
	}
	else
	{
		//parse the file to get the saved settings
		std::string CurLine;
		while (std::getline(ConfigIn, CurLine)) // gets the lines from the file
		{
			bool Done = false;
			std::string CurWord = "";
			std::string Property = "";
			std::string Value = "";

			bool EqualsFound = false;

			int i = 0;
			while (!Done) // parses the current line
			{	
				char CurChar = CurLine.at(i);

				if (EqualsFound)
				{
					if (CurChar == ';')
					{
						Value = CurWord;
						CurWord = "";
						Done = true;
					}
					else
					{
						CurWord += CurChar;
					}
				}
				else
				{
					if (CurChar == '=')
					{
						EqualsFound = true;
						Property = CurWord;
						CurWord = "";
					}
					else
					{
						CurWord += CurChar;
					}
				}
				
				i++;
				if (i >= CurLine.size()) // ignores invalid lines using this
				{
					Done = true;
				}
			}

			//if both property and value are valid
			if (Property != "")
			{
				ConfigMap.insert(std::make_pair(Property, Value));
			}
		}

		ConfigIn.close();
	}


}
