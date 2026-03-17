// $Id$
//
// A base class for event data containers that are based on STL vectors.
//
#ifndef T_DATA_VECTOR_HXX
#define T_DATA_VECTOR_HXX

#include <iterator>
#include <vector>

#include "TDatum.hxx"
#include "TData.hxx"
#include "TND280Log.hxx"

#define T_DATA_VECTOR_TITLE "Data Vector"

class TBrowser;

namespace ND {
    typedef std::vector<TDatum*> TDatumVector;
    class TDataVector;
    template <typename BaseIterator, typename Type> class TDataVectorIterator;
}

/// A flexible container for event data that is part of a TDatum name space
/// with data access provided by TDatum::Use(), and TDatum::Get().  This
/// particular class has the properties of a dictionary (which is turn is a
/// conceptual map, not a C++ std::map).  Although the elements of the
/// TDataVector can be accessed like elements of a standard vector, they are
/// generally accessed by name using the TDatum::Use() or TDatum::Get()
/// methods.  See the base TDatum class for more documentation on those
/// methods.  The TDatum::Use() method is probably the most applicable to user
/// code, but be careful since it will through a ENoSuchElement exception if
/// the requested name doesn't exist.  The existence of objects can be checked
/// for using the TDatum::Has() method.  As an example of a typical usage
/// might be:
///
/// \code
/// int trajectoryCount(TND280Event& evt) {
///     try {
///         TG4TrajectoryContainer& trajectories
///             = evt.Use<TG4TrajectoryContainer>("~/truth/G4Trajectories");
///         return trajectories.size();
///     }
///     catch (ENoSuchElement) {
///         return 0;
///     }
/// }
/// \endcode
///
/// The TDataVector class is extremely useful in building data hierarchies of
/// reconstruction results.  Any object derived from TDatum can be added to a
/// TDataVector by name and then accessed in a "type-safe" manner using the
/// TDatum::Use() or TDatum::Get() methods.
///
/// Any TDatum derived object can be added to a TDataVector object.  The
/// preferred way to add a TDatum derived object is to use the AddDatum method
/// which comes in two flavors.  The first flavor takes a pointer to a TDatum
/// object, and assumes ownership of the objects (as per the memory management
/// rules).  The second form of AddDatum takes a handle to a TDatum object,
/// this has slightly unusual semantics since it takes ownership of the object
/// away from the ND::THandle so that the data pointed two by the ND::THandle
/// is now owned by the ND::TDataVector object.  Objects added to the
/// ND::TDataVector::AddDatum method will be saved when the TDataVector is
/// written to an output file (this is the main way that data is saved to a
/// file).
///
/// In addition to AddDatum, any TDatum derived object can be added to the
/// TDataVector object using ND::TDataVector::AddTemporary.  Objects added in
/// this way will not be saved to the output file.  You can check if an object
/// is placed in the temporary store by using the
/// ND::TDataVector::IsTemporary method.
class ND::TDataVector : public TData {
    template <typename BaseIterator, typename Type> 
    friend class TDataVectorIterator;

private:
    /// Copying a TDataVector is illegal.  A TDataVector cannot be copied
    /// since the members are pointers to other TDataVector objects.  A copy
    /// of a TDataVector would result in double deletes of objects, or require
    /// that the object is not completely copied.  Since neither option is
    /// acceptable (i.e. illegal, or doesn't meet the required symantics for a
    /// copy), there is no copy constructor.
    TDataVector(const TDataVector&);

    /// Assigning a TDataVector is illegal.  Assignment is illegal for the
    /// same reasons as the copy constructor.
    TDataVector& operator = (const TDataVector&);

public:
    /// @{Bidirectional iterators as per STL.  Bidirectional iterators to
    /// sequence through the elements of the TDataVector from beginning to
    /// end.
    typedef TDataVectorIterator<ND::TDatumVector::iterator,
                                ND::TDatum*> iterator;
    typedef TDataVectorIterator<ND::TDatumVector::const_iterator,
                                ND::TDatum* const >  const_iterator;
    /// @}

    /// @{Bidirectional reverse iterators as per STL.  Iterators to sequence
    /// through the elements of the TDataVector in reverse.
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    /// @}

    TDataVector() : TData("",T_DATA_VECTOR_TITLE) { };

    /// Create a new TDataVector.  A TDataVector inherits two fields from
    /// TNamed, "Name" and "Title".  The name is the handle that is
    /// used to find "this" in TDataVector objects.  It can only contain
    /// alphanumeric characters (no "/" or spaces) and *should* be
    /// unique within any level of the TDataVector hierarchy.  For instance,
    /// to get a particular datum out of a TDataVector object you use
    /// data->Get<TDatumClass>("thename").  The title is a description
    /// of a particular type of data.  As an example, the MC
    /// interaction data might be named "interaction" and titled "MC
    /// Interaction Data".  The default title is "Event Data Vector".
    explicit TDataVector(const char* name,
                         const char* title = T_DATA_VECTOR_TITLE)  
        : TData(name,title) { };

    /// This will recursively delete the data vector *AND* all of the
    /// children.
    virtual ~TDataVector();

    /// A virtual method that is used to find data in this.  The Get member
    /// template will use FindDatum to implement the error checked access.
    /// This is a pure virtual function since it must be implemented.
    virtual TDatum* FindDatum(const char* name);

    /// Add a temporary TDatum that will not be saved to the output file.
    /// This is intended to allow analysis code to add temporary data to a
    /// TDataVector derived object that will persist as long as the object is
    /// in memory, but which is not saved to the output file.  This can be
    /// used to cache results that are expensive to calculate (i.e. slow), but
    /// which can be rederived from information being saved in the object.
    /// The temporary object store is searched before the primary object
    /// store.
    virtual void AddTemporary(TDatum* val, const char* name=NULL);
    
    /// Add a temporary TDatum that will not be saved to the output file.
    /// This takes over memory management of the handle and will throw the
    /// EBadInsertion exception if the insertion fails.
    virtual void AddTemporary(ND::THandle<ND::TDatum> handle,
                              const char* name=NULL);

    /// @{Check if a TDatum object is temporary in this data vector.
    virtual bool IsTemporary(TDatum* val);
    virtual bool IsTemporary(ND::THandle<TDatum> val);
    /// @}

    /// @{As per the STL.  Return iterators to sequence through the vector
    /// from beginning to end.  The mutable form is iterator and allows the
    /// referenced object to be changed.  The constant form is const_iterator
    /// and references a constant TDatum pointer.
    virtual iterator begin(void);
    virtual iterator end(void);
    virtual const_iterator begin(void) const;
    virtual const_iterator end(void) const;
    /// @}

    /// @{As per the STL.  Return iterators to sequence through the vector in
    /// revesrbeginning to end.  The mutable form is reverse_iterator and
    /// allows the referenced object to be changed.  The constant form is
    /// const_reverse_iterator and references a constant TDatum pointer.
    virtual reverse_iterator rbegin(void);
    virtual reverse_iterator rend(void);
    virtual const_reverse_iterator rbegin(void) const;
    virtual const_reverse_iterator rend(void) const;
    /// @}

    /// Add a TDatum derived object to the end of the vector and pass
    /// ownership to the vector.  This can only insert persistent objects so
    /// use AddTemporary to insert non-persistent objects.
    virtual void push_back(TDatum* data);

    /// @{Remove a TDatum object from the TDataVector by finding the pointer and
    /// then removing.  This returns an iterator to the element that immediately
    /// followed the object at position (as per the STL vector erase).
    virtual iterator erase(TDatum* position);
    virtual iterator erase(ND::THandle<ND::TDatum> position);
    /// @}

    /// As per STL.  Remove a TDatum object from the TDataVector based on an
    /// iterator position.  This returns an iterator to the element
    /// immediately following the position.
    virtual iterator erase(iterator position);

    /// As per STL.  Insert a TDatum object into the TDataVector before
    /// position.  Ownership of the object is passed to the TDataVector.  This
    /// method can not be used to insert a temporary object.
    virtual iterator insert(iterator position, TDatum* element);

    /// A templated method to return the n'th object in the std::vector.  This
    /// is not a recommended way to access a TDataVector, but can be useful
    /// when a TDataVector has been beaten into service.  If you find that you
    /// need this, you should consider writing a special purpose class similar
    /// to TTrackContainer.  Usually, you should use the Get() method and
    /// access TDataVector elements by name.
    template <class T> T* At(int num) const {
        TDatum *d = operator[](num);
        T* t = dynamic_cast<T*>(d->GetThis());
        if (!t) {
            ND280Warn("TDatum::Get<" << ClassName() 
                      << ">:Cannot convert class "
                      << ClassName()
                      << " to class " << T::Class_Name());
            throw EBadConversion();
        }
        return t;
    }

    /// Return the n'th TDatum in the TDataVector.  As with the At() method,
    /// this is not the recommended method of accessing TDataVector elements.
    /// Usually, you should use the Get() method and access TDataVector
    /// elements by name.
    virtual TDatum* operator [](int num) const {
        if (num < (int) fVector.size()) return fVector[num];
        if (num < (int) size()) return fTemporary[num-fVector.size()];
        throw ENoSuchElement();
    }

    /// Return the number of elements in the TDataVector.
    virtual unsigned int size(void) const {
        return fVector.size()+fTemporary.size();
    }

    /// @{Override methods in the base TObject class.
    virtual void ls(Option_t* opt = "") const;
    virtual void Clear(Option_t *option="");
    virtual Bool_t IsFolder(void) const {return kTRUE;};
    virtual void Browse(TBrowser* b);
    /// @}

protected:
    /// Remove the element from the container.  The object can be deleted
    /// after this has been called, or it can be inserted someplace else.  If
    /// the element is found, then return value is equal to element.
    /// Otherwise it's NULL.  See ND::TDatum::RemoveDatum() for details.
    virtual TDatum* RemoveDatum(TDatum* element);

    /// Insert the element into the container and set the parent.  The element
    /// is inserted at the end of the container.
    virtual void InsertDatum(TDatum* val);

    /// Insert the object into the temporary storage.
    virtual void InsertTemporary(TDatum* val);

private: 
    TDatumVector fVector;
    
    /// Hold all of the temporary objects.  These objects are not saved in the
    /// output file.
    TDatumVector fTemporary; //! Do Not Save

    ClassDef(TDataVector,4);
};

/// As per the STL.
namespace ND {
    template <typename BaseIterator, typename Type>
    class TDataVectorIterator {
        TDataVectorIterator(const ND::TDataVector* obj,
                            const BaseIterator current) {
            // Cast to a non-const version since both the const and non-const
            // version of begin are required.
            fObject = const_cast<ND::TDataVector*>(obj);
            fCurrent = current;
            if (fCurrent == fObject->fVector.end()) {
                fCurrent = fObject->fTemporary.begin();
            }
        }
        ND::TDataVector* fObject;
        BaseIterator fCurrent;

    public:

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = Type*;
        using reference = Type&;


        friend class ND::TDataVector;

        /// @{As per the STL definitions for a bidirectional iterator.
        Type& operator*() const {return *fCurrent;}
        Type* operator->() const {return &(*fCurrent);}
        bool operator==(const TDataVectorIterator &rhs) const {
            if (fObject != rhs.fObject) return false;
            if (fCurrent != rhs.fCurrent) return false;
            return true;
        }
        bool operator!=(const TDataVectorIterator &rhs) const {
            return !((*this) == rhs);
        }
        TDataVectorIterator& operator++() {
            ++fCurrent;
            if (fCurrent == fObject->fVector.end()) {
                fCurrent = fObject->fTemporary.begin();
            }
            return *this;
        }
        
        TDataVectorIterator operator++(int) {
            TDataVectorIterator tmp(*this);
            ++(*this);
            return tmp;
        }
        
        TDataVectorIterator& operator--() {
            if (fCurrent == fObject->fTemporary.begin()) {
                fCurrent = fObject->fVector.end();
            }
            --fCurrent;
            return *this;
        }
        
        TDataVectorIterator operator--(int) {
            TDataVectorIterator tmp(*this);
            --(*this);
            return tmp;
        }
        /// @}
    };
}
#endif
