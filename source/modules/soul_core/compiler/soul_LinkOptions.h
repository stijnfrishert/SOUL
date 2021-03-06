/*
    _____ _____ _____ __
   |   __|     |  |  |  |      The SOUL language
   |__   |  |  |  |  |  |__    Copyright (c) 2019 - ROLI Ltd.
   |_____|_____|_____|_____|

   The code in this file is provided under the terms of the ISC license:

   Permission to use, copy, modify, and/or distribute this software for any purpose
   with or without fee is hereby granted, provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

namespace soul
{

//==============================================================================
/**
    A set of named properties that are passed into the linker and performers.
*/
struct LinkOptions  : public Annotation
{
    LinkOptions (SampleRateAndBlockSize rateAndBlockSize)   { setSampleRateAndBlockSize (rateAndBlockSize); }
    LinkOptions (double sampleRate, uint32_t blockSize) : LinkOptions (SampleRateAndBlockSize (sampleRate, blockSize)) {}

    //==============================================================================
    static constexpr const char* getOptimisationLevelKey()  { return "optimisation_level"; }
    void setOptimisationLevel (int level)                   { set (getOptimisationLevelKey(), (int32_t) level); }
    int getOptimisationLevel() const                        { return (int) getInt64 (getOptimisationLevelKey(), -1); }

    //==============================================================================
    static constexpr const char* getMaxStateSizeKey()       { return "max_state_size"; }
    void setMaxStateSize (uint64_t size)                    { if (size > 0) set (getMaxStateSizeKey(), (int64_t) size); }
    uint64_t getMaxStateSize() const                        { return (uint64_t) getInt64 (getMaxStateSizeKey(), defaultMaximumStateSize); }

    //==============================================================================
    static constexpr const char* getMainProcessorKey()      { return "main_processor"; }
    void setMainProcessor (const std::string& name)         { set (getMainProcessorKey(), name); }
    std::string getMainProcessor() const                    { return getString (getMainProcessorKey()); }

    //==============================================================================
    static constexpr const char* getPlatformKey()           { return "platform"; }
    void setPlatform (const std::string& name)              { set (getPlatformKey(), name); }
    std::string getPlatform() const                         { return getString (getPlatformKey()); }

    //==============================================================================
    static constexpr const char* getSessionIDKey()          { return "sessionID"; }
    void setSessionID (int32_t sessionID)                   { set (getSessionIDKey(), sessionID); }
    int32_t getSessionID() const                            { return int32_t (getInt64 (getSessionIDKey())); }
    bool hasSessionID() const                               { return hasValue (getSessionIDKey()); }

    //==============================================================================
    static constexpr const char* getBlockSizeKey()          { return "blockSize"; }
    void setBlockSize (uint32_t blockSize)                  { set (getBlockSizeKey(), (int32_t) blockSize); }
    uint32_t getBlockSize() const                           { return (uint32_t) getInt64 (getBlockSizeKey()); }

    //==============================================================================
    static constexpr const char* getSampleRateKey()         { return "sample_rate"; }
    void setSampleRate (double sampleRate)                  { if (sampleRate > 0) set (getSampleRateKey(), sampleRate); }
    double getSampleRate() const                            { return getDouble (getSampleRateKey()); }

    void setSampleRateAndBlockSize (SampleRateAndBlockSize rateAndBlockSize)
    {
        setSampleRate (rateAndBlockSize.sampleRate);
        setBlockSize (rateAndBlockSize.blockSize);
    }

    SampleRateAndBlockSize getSampleRateAndBlockSize() const
    {
        return { getSampleRate(), getBlockSize() };
    }

    //==============================================================================
    using ExternalValueProviderFn = std::function<ConstantTable::Handle (ConstantTable&,
                                                                         const char* name,
                                                                         const Type& requiredType,
                                                                         const Annotation& annotation)>;

    /** If this lamdba is set, it must return the Value that should be bound to a
        given external variable. The name provided will be fully-qualified, and the Value
        returned must match the given type, or an error will be thrown.
    */
    ExternalValueProviderFn externalValueProvider;

    //==============================================================================
    static constexpr uint64_t  defaultMaximumStateSize = 1024 * 1024 * 20;
};

//==============================================================================
/**
    Provides a mechanism that a Performer may use to store and retrieve reusable
    chunks of binary code, to avoid re-compiling things multiple times.

    An implementation just has to store chunks of data for particular string keys. That
    could be done in some kind of file structure or database depending on the use-case.
*/
class LinkerCache
{
public:
    virtual ~LinkerCache() {}

    /** Copies a block of data into the cache with a given key.
        The key will be an alphanumeric hash string of some kind. If there's already a
        matching key in the cache, this should overwrite it with the new data.
        The sourceData pointer will not be null, and the size will be greater than zero.
    */
    virtual void storeItem (const char* key, const void* sourceData, uint64_t size) = 0;

    /**
        The key will be an alphanumeric hash string that was previously used to store the item.
        If destAddress is nullptr or destSize is too small, then this should return the size
        that is required to hold this object.
        If no entry is found for this key, the method returns 0.
    */
    virtual uint64_t readItem (const char* key, void* destAddress, uint64_t destSize) = 0;
};

}
