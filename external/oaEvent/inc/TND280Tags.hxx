#ifndef TND280Tags_hxx_seen
#define TND280Tags_hxx_seen
#include "TND280Event.hxx"
#include "TDataVector.hxx"
#include "THandle.hxx"
#include "TSHAHashValue.hxx"
namespace ND {
	class TND280Tags;
	class TND280TagControl;
}
/*! Save arbitary "tags" as key-value pairs.
For example:
\code
 ND::TND280Tags* tags = new ND::TND280Tags("MCTags","Monte Carlo Generator Tags");
 tags->AddTag("NEUT Version","5.4.0");
 tags->AddTag("NEUT Parameter of somesuch","xyz");
 ND::THandle<ND::TDataVector> vect = event.Get<ND::TDataVector>("tags");
		if(vect)
		{
			vect->AddDatum(tags);
		}
\endcode
See also \ref addingND280tags
*/
class ND::TND280Tags: public ND::TDataVector {
public:
	/// A map for storing pairs of key:value pairs.
	typedef std::map<std::string,std::string> tagsMap;

	/// Default constructor sets Name =  = "ND280Tags", and Title "Empty"
	TND280Tags();
	/// Set Name and Title
	TND280Tags(const char* name, const char* title);
	/// Set Name and Title and initialise map to a copy of a pre-existing TND280Tags::tagsMap
	TND280Tags(const char* name, const char* title,tagsMap map);
	/// Add a tag to the map
	void AddTag(std::string key,std::string value){fTags[key]=value;};
	/// Return a copy of the internal tags  map
	tagsMap GetMap(){return fTags;};
	virtual ~TND280Tags(){};   
    /// Print the information about this ND280Tags object.
	virtual void ls(Option_t *opt = "") const;
	/// Set the tagsMap equal to an existing one.
	void SetTags(tagsMap configurationMap){fTags=configurationMap;};

private:

	tagsMap fTags; //!< Collection of key=value pairs ( all std::string s)
	

	ClassDef(TND280Tags,1);

};
/// A class for controlling TND280Tags. It keeps the TND280Tags clean, avoiding static member functions therein.
/// It contains functions for automatically adding information about program version numbers to the
/// /tags/jobs part of an event and for automatically adding information read from an external text file.
/// Normally just one of these objects is needed per program, but it is not explicitly made a singleton class.
/// This means that for example multiple configuration files could be read in if needed.
class ND::TND280TagControl 
{

private:

	// Copy of argc from command line
	static int argc;
	// Copy of argv from command line
	static char** argv;
	// store configuration information read from a file
	ND::TND280Tags::tagsMap fConfigurationMap;
	// Title to use for configuration information
	TString fConfigurationTitle;
	// Name of file used to read in configuration information
	TString fConfigurationMapFileName;

public:
	/// If necessary add the /tags top level DataVector to the event.
	void Initialise(ND::TND280Event& event);
	/// Remember the command line used to start this program so we can later store it in the /tags/jobs area.
	/// (see addProgramVersionNumberTags)
	void StoreCommandLine(int argcIn,char** argvIn){argc=argcIn;argv=argvIn;};
	/// Add configuraton information previously read in from a file by readTagsFromFile.
	void AddConfiguration(ND::TND280Event& event);
	/// Read in key=value pairs from a file and store them for later adding to events by addConfiguration.
	void ReadTagsFromFile(TString filename);
	/// Automatically add information about library versions to the /tags/jobs area of an event.
	void AddProgramVersionNumberTags(ND::TND280Event& event);
	/// Reqired virtual destructor
	virtual ~TND280TagControl(){};

	ClassDef(TND280TagControl,1);

};
#endif
