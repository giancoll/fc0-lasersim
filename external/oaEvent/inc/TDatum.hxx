// $Id$
//
// The base class for Event Data.
//
#ifndef T_DATUM_HXX
#define T_DATUM_HXX

#include <iostream>
#include <vector>

// Include the ROOT base classes.
#include <TROOT.h>
#include <TNamed.h>
#include <TQObject.h>

#include "THandle.hxx"
#include "EoaCore.hxx"
#include "TND280Log.hxx"

namespace ND {
    OA_EXCEPTION(EDatum,EoaCore);
    OA_EXCEPTION(EBadConversion,EDatum);
    OA_EXCEPTION(EBadName,EDatum);
    OA_EXCEPTION(ENoSuchElement,EDatum);
    class TDatum;
    class TDatumCompareName;
    class TData;
    class TDataVector;
    class TDataSymLink;
    class TDataLink;

    /// A function to print the header for the ls method.  This is provided so
    /// that all classes in oaEvent that want to customize the ls() method can
    /// print a standard header.  This does not append a std::endl to the
    /// output.
    void ls_header(const TObject* obj, Option_t* opt);
}

#define TDATUM_TITLE "Event Datum"

/// The base class for ND280 data with access provided using the Use() and
/// Get() methods.  In some ways this is analogous to the root TObject class,
/// except that this is specialize for saving event to a file.  A TDatum
/// represents a single piece of information.  This is supplemented by the
/// TDataVector (which is derived from TData) which is used to represent
/// hierarchies of data and is similar in concept to a TFolder or a
/// TDirectory.  The main difference between TObject (TFolder) and TDatum
/// (TDataVector) is that a TDatum object maintains information about the
/// object which owns it "this".  As a result, data hierarchies built using
/// TDatum objects do not need to use global variables for navigation so
/// multiple hierarchies can be trivially handled.
///
/// A TDatum event is the base class (similar to TObject) of several classes
/// for saving data.  It can be used as a direct parent for any class saving
/// "scalar" data, or which does not have any child "TDatum" classes.  The
/// TDataVector class is derived from TDatum and is intended to help save
/// hierarchies of data.  Objects that are derived from the TDatum object can
/// be used to constructed a "virtual file system" of information.  This
/// allows arbitrary information to be saved with an event.
///
/// When working with TDataVector and TDatum derived objects, you can think of
/// the structure as a file system with directories and files.  The TDatum
/// objects are the files.  The TDataVector objects are the directories, but
/// notice that TDataVector derives from TDatum, so "directories" are also
/// "files".  You can access "files" (i.e. any TDatum derived objects) below a
/// current object using the Use(), or Get() methods.  You can check for the
/// existence of a "file" using the Has() method.
class ND::TDatum : public TNamed {
    friend class TData;
    friend class TDataVector;
    friend class TDataSymLink;
    friend class TDataLink;

public:
    TDatum();

    /// Create a new TDatum.  A TDatum inherits two fields from TNamed, "Name"
    /// and "Title".  The name is the handle that is used to find "this" in
    /// TData objects.  It can only contain alphanumeric characters (no "/" or
    /// spaces) and *should* be unique with in any level of the TData
    /// hierarchy.  For instance, to get a particular datum out of a TData
    /// object you use data->Get<TDatumClass>("thename").  The title is a
    /// discription of a particular type of datum.  As an example, the MC
    /// interaction data might be named "interaction" and titled "MC
    /// Interaction Data".  The default title is "Event Datum".
    explicit TDatum(const char* name, const char* title = TDATUM_TITLE);

    virtual ~TDatum();

    /// Return the full name of this datum.  The full name places the
    /// datum in the data tree and has the syntax //xxxx/yyyy/zzzz. 
    virtual TString GetFullName(void) const;

    //@{
    /// Do a dynamic_cast of a TDatum to a derived class and provide a pointer
    /// to the object.  This uses a recusive search so that it can return the
    /// current object as a derived class, or search for an object of the
    /// specified name.  For instance, the name might be "dir/subdir/object",
    /// or "../dir/object".  If the name does not exist, this returns NULL.
    ///
    /// The name of the requested TDatum can be specified as follows:
    ///
    ///   -# specify the current object using "."
    ///   -# specify the parent of the object using ".."
    ///   -# specify a pathname relative to the root of the tree using "~/".
    ///   (e.g. "~/xxx/yyy/name")  
    ///   -# specify a full pathname starting at the top of the
    ///   heirachy.  (eg. "//root/xxx/yyy/name"
    ///   -# specify a pathname starting with a single slash.  This
    ///   searches up to find the first occurance of xxx. (eg. "/xxx/name")
    ///   -# Specify a pathname relative to this folder. (e.g. "xxx/yyy/name")
    ///   This only works when the current object is derived from TData
    ///   (which is derived from TDatum).
    ///
    /// This should look very familiar since it is based on unix file naming
    /// conventions.  A typical usage might look something like this:
    ///
    /// \code
    ///  myFunc(TND280Event& evt) {
    ///      THandle<TG4TrajectoryContainer> trajectories 
    ///          = evt.Get<TG4TrajectoryContainer>("truth/G4Trajectories");
    ///      if (trajectories) {
    ///          std::cout<< trajectories->GetName() << std::endl;
    ///      }
    ///      else {
    ///          std::cout << "Element is not found" << std::endl;
    ///      }
    ///  }
    /// \endcode
    /// 
    /// If the name is not found in the event, then this returns a NULL
    /// pointer.  
    /// 
    /// This member method template handles the type conversions and
    /// helps to insure type safety.  The primary purpose of this method is to
    /// insure that the proper downcasts can occur and if they cannot, then
    /// handle the errors correctly.  This throws an EBadConversion exception
    /// if the conversion cannot occur.  This is essentially just a dynamic
    /// cast, but it has the advantage of having a little bit of error
    /// checking built in and is formalizing the casting policy for this
    /// library.  Notice that this has link in direction built in if the
    /// referenced object is a link reference class. WARNING: This cannot be
    /// used interactively inside of early root versions.
    template <class T> ND::THandle<T> Get(const char* name=".") const {
        TDatum *d = RecursiveFind(name);
        if (!d) return THandle<T>(NULL);
        T* t = dynamic_cast<T*>(d->GetThis());
        if (!t) {
            TObject* tmp = d->GetThis();
            if (!tmp) tmp = d;
            ND280Warn(ClassName() << "::Get<" << T::Class_Name() 
                      << ">(" << name << "): Cannot convert from"
                      << " \"" << tmp->ClassName() << "\""
                      << " to \"" << T::Class_Name() << "\""
                      << std::endl
                      << "    Object name:"
                      << " \"" << d->GetFullName() << "\"");
            throw EBadConversion();
        }
        return THandle<T>(t,false);
    }

    template <class T> ND::THandle<T> Get(const std::string& name) const {
        return Get<T>(name.c_str());
    }
    //@}

    /// Check that a reference to an element the name space (see the Get()
    /// method for naming syntax details).  If the element is not found, then
    /// an ENoSuchElement exception will be thrown.  This throws an
    /// EBadConversion if the element cannot be dynamic cast to the requested
    /// class.  A typical usage might look something like this:
    ///
    /// \code
    ///  myFunc(TND280Event& evt) {
    ///      try {
    ///          TG4TrajectoryContainer& trajectories 
    ///              = evt.Use<TG4TrajectoryContainer>("truth/G4Trajectories");
    ///          std::cout << trajectories.GetName();
    ///      }
    ///      catch (ENoSuchElement) {
    ///          std::cout << "Element is not found" << std::endl;
    ///      }
    ///  }
    /// \endcode
    /// 
    /// If the name is not found in the event, then this returns a NULL
    /// pointer.  

    template <class T> T& Use(const std::string& name) const {
        THandle<T> object(Get<T>(name));
        if (!object) throw ENoSuchElement();
        return *object;
    }

    /// Check that a particular element exists in the name space (see the
    /// Get() method for naming syntax details).
    template <class T> bool Has(const std::string& name) const {
        TDatum *d = RecursiveFind(name.c_str());
        if (!d) return false;
        T* t = dynamic_cast<T*>(d->GetThis());
        if (!t) return false;
        return true;
    }

    /// A virtual method that is used to find data in "this".  The
    /// derived class will provide a FindDatum that understands the
    /// specific container used and the Get member template will use
    /// FindDatum to implement the error checked access.  The Get
    /// member template will also implement a search through a name
    /// hierarchy.  However, the FindDatum method should only attempt
    /// to find things in the particular container.
    virtual TDatum* FindDatum(const char* name);

    /// Print the datum information.
    virtual void ls(Option_t* opt = "") const;

    /// Called by the browser.
    virtual void Browse(TBrowser* b);

protected:
    /// @{A virtual function that can be used to have "special" Get<>
    /// behavior.  This is mostly used by the TDatumLink class so that
    /// it can have a weak reference to TDatum elsewhere in the
    /// hierarchy, and have a Get<> method that refers transparently to
    /// the linked object.  The GetThis() method should be used
    /// wherever the link indirection might be desired.
    virtual const TDatum *GetThis(void) const {return this;};
    virtual TDatum *GetThis(void) {return this;};
    /// @}

    /// Used by derived classes to set the parent.  If the Datum has a
    /// parent, then this will use RemoveDatum to take the datum out of
    /// the parent before the parent field is set to the new value.
    /// This is mainly used by the TData derived classes to set the
    /// parent as TDatum added and removed from the TData collections.
    /// This method should never be directly used outside of a derived
    /// class, and the fact that it is public is because of a design
    /// problem.  It should be reworked and the derived classes
    /// modified to meet the new spec so that this can be done as a
    /// protected member.
    virtual void ReassignParentDatum(TDatum *parent);
    
    /// Used by derived classes to directly assign the parent.  This
    /// is probably needed because of a design error.  This is
    /// different in that ReassignParentDatum because this will not try to
    /// remove the datum from the parent before setting the parent
    /// field.
    virtual void AssignParentDatum(TDatum *parent) {fParent = parent;};

    /// Used by the Get<> method template to find the actual object to
    /// return.  For instance, the name might be "dir/subdir/object",
    /// or "../dir/object".  See TDatum::Get() for details.
    virtual TDatum *RecursiveFind(const char *name) const;

    /// Get the parent of this Datum
    virtual TDatum* GetParentDatum(void) const {return fParent;};

    /// Do a "dynamic_cast" of the parent.  This is better than a pure
    /// dynamic class since there is error reporting built in, and if
    /// we ever decide to use exceptions this gives a single point to
    /// add them.  It also makes the user code much clearer since it
    /// can be seen that the Parent is being accessed as a particular
    /// type.  WARNING: This cannot be used inside of root
    /// interactively.
    template <class T> ND::THandle<T> GetParent(void) const{
        if (!fParent) {
            return ND::THandle<T>(NULL);
        }
        T* p = dynamic_cast<T*>(fParent);
        if (!p) {
            // This might not work if T is not derived from a TObject,
            // but I think all will be well.
            ND280Warn("TDatum::GetParent:Cannot convert a TDatum to " 
                      << T::Class_Name());
            throw EBadConversion();
        }
        return ND::THandle<T>(p,false);
    }
    
    /// Get the datum at the top of the TDatum tree.
    virtual TDatum* GetRootDatum(void) const;

    /// This method checks to see if the element is contained in this datum
    /// (e.g. *this as a parent).  If this datum is the parent, the element is
    /// removed and it's parent pointer is set to NULL.  It should be
    /// implemented in any class derived from TDatum that contains elements.
    /// This is used internally by ReassignParent help move the data.  The
    /// return value is the mutated element.  If the element is not owned by
    /// this datum, then it is not changed. 
    /// 
    /// \return The return value will either be NULL, or a pointer to element.
    /// If the return value is NULL, then the element was not found in this
    /// object, and ownership is not transferred to the caller.  If the
    /// element is found, the return value is a pointer to the element, and
    /// ownership is transferred to the caller.
    virtual TDatum* RemoveDatum(TDatum* element);

private:
    /// The parent which owns this.
    TDatum *fParent;

    ClassDef(TDatum,2);
};

/// A functor used to compare the name of a TDatum against a target
/// name.  This is used in the find_if template. 
class ND::TDatumCompareName {
private:
    TString fName;
public:
    explicit TDatumCompareName(const char* name);
    ~TDatumCompareName();
    bool operator ()(const TDatum* elem);
};

#endif
