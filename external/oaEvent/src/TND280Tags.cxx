#include "TND280Tags.hxx"
#include "TOADatabase.hxx"
#include "HEPUnits.hxx"
#include <iostream>
#include <fstream>
#include <sstream>


int ND::TND280TagControl::argc{0};
char** ND::TND280TagControl::argv{NULL};
ClassImp(ND::TND280Tags);
ClassImp(ND::TND280TagControl);
ND::TND280Tags::TND280Tags() : ND::TDataVector("ND280Tags", "Empty") {}
ND::TND280Tags::TND280Tags(const char* name, const char* title)
: ND::TDataVector(name,title) {}
ND::TND280Tags::TND280Tags(const char* name, const char* title,tagsMap map)
: ND::TDataVector(name,title), fTags(map){}
void ND::TND280Tags::ls(Option_t* opt) const {
	TROOT::IncreaseDirLevel();
	ND::TDatum::ls(opt);
	TROOT::IncreaseDirLevel();
	TROOT::IndentLevel();
	std::cout<<GetTitle()<<std::endl;
	for (  std::pair<std::string const, std::string> tag : fTags )
	{
		TROOT::IndentLevel();
		std::cout<<tag.first<<" = "<<tag.second<<std::endl;
	}
	TROOT::DecreaseDirLevel();
	TROOT::DecreaseDirLevel();
}
void ND::TND280TagControl::Initialise(ND::TND280Event& event)
{
	if (!event.FindDatum("tags")) 
		event.AddDatum(new ND::TDataVector("tags","Generator information etc."));
}
void ND::TND280TagControl::AddProgramVersionNumberTags(ND::TND280Event& event){
	Initialise(event);
// Find the name of the program used to launch this program
//
// Either from /proc/self/exe or from the command line 
// information if that has been set in advance.
//
	std::string name;
	std::string arguments;
	if(argc==0)
	{
// Get name of program ( truncated after 15 characters )
		std::ifstream comm("/proc/self/exe");
		getline(comm, name);
	}
	else
	{
// remember name of executable and values of arguments for later use
		name=argv[0];
		for(int i=1;i<argc;i++)
		{
			arguments=arguments+" "+argv[i];
		}
	}
	ND::TND280Tags* tags = new ND::TND280Tags(name.c_str(),std::string(name+" "+arguments).c_str());

	ND::TPackageSet  packages =   ND::TOADatabase::Get().PackageSet();

	for (ND::TPackageVersion* package : packages)
	{
		tags->AddTag(package->GetName(),package->GetVersion());
	}
	ND::THandle<ND::TDataVector> tagsVector = event.Get<ND::TDataVector>("tags");
	if (!tagsVector->FindDatum("jobs")) {
		tagsVector->AddDatum(new ND::TDataVector("jobs","Jobs audit trail"));
	}
	ND::THandle<ND::TDataVector> jobs = tagsVector->Get<ND::TDataVector>("jobs");
	if(jobs)
	{
		jobs->AddDatum(tags);
	}
}
void ND::TND280TagControl::ReadTagsFromFile(TString fileName){
	std::ifstream file(fileName);
	if (! file.is_open())
	{
		ND280Error("Failed to open file containing tags named"<<fileName);
		return ;
	}
	fConfigurationMapFileName=fileName;
	fConfigurationTitle=TString("Configuration from ")+fileName;
	std::string line;
	while( std::getline(file, line) )
	{
// ignore lines which start with #
		if(line[0]!='#')
		{
// create a stream from the line read in
			std::istringstream is_line(line);
			std::string key;
// read in everything up to the = sign
			if( std::getline(is_line, key, '=') )
			{
				std::string value;
// read everything after the = sign
				if( std::getline(is_line, value) )
// store  the key value pair 
					fConfigurationMap[key]=value;
			}
		}
	}
	return ;
}
void ND::TND280TagControl::AddConfiguration(ND::TND280Event& event)
{
	if(fConfigurationMap.size()>0)
	{
		Initialise(event);
		ND::THandle<ND::TDataVector> vect = event.Get<ND::TDataVector>("tags");
		if(vect)
		{
			vect->AddDatum(new ND::TND280Tags("Configuration",fConfigurationTitle,fConfigurationMap));
		}
	}
}