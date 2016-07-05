/** \file	XFilters.h */
#ifndef _XENICS_XFILTERS_H_
#   define _XENICS_XFILTERS_H_

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Filters, query available list / start
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#   include <XCamera.h>

    /// Generates a comma separated list of image filters useable in QueueFilter
    /// @param cam          - camera class instance
    /// @param fltlist      - string buffer
    /// @param fltlistmax   - bytes allocated
    ErrCode  IMPEXP GetFilterList   (XCamera *cam, char *fltlist, int fltlistmax);

    /// Queue's the given filter, if found, identified by the string filtername which was retrieved from the GetFilterList function.
    /// @param cam          - camera class instance
    /// @param fltname      - filter name @sa GetFilterList
    /// @param fltparms     - startup parameters (not available for all filters)
    /// @return               Returns the id to the new queued filter. When a negative return value is returned, queueing the filter failed.
    // Examples:
    // FilterID fID = QueueFilter(cam, "SoftwareCorrection", "");
    // FilterID fID = QueueFilter(cam, "Thermography"      , "unit:celsius");
    FilterID IMPEXP QueueFilter     (XCamera *cam, const char *fltname, const char *fltparms);

#   ifdef __cplusplus
    extern "C" 
    {
#   endif

        ErrCode  IMPEXPC XC_FLT_GetFilterList (XCHANDLE hnd, char *fltlist, int fltlistmax);              ///< @sa GetFilterList
        FilterID IMPEXPC XC_FLT_Queue         (XCHANDLE hnd, const char *fltname, const char *fltparms);  ///< @sa QueueFilter

#   ifdef __cplusplus
    }
#   endif


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Filter data structures
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////
    /// XFilters - Xenics image filters
    //////////////////////////////////////////////////////////////////////////////////// 

    //////////////////////////////////
    // Information exchange messages
    ////////////////////////////////

    /// Fetch a point value from the temperature filter
    typedef struct {
        dword   x;          ///< X coordinate
        dword   y;          ///< Y coordinate
        double  e;          ///< Emissivity
        double  value;      ///< Resulting value
    } XMsgGetTempData;

    /// Ask a filter to convert an adu value to temperature
    typedef struct {
        dword   adu;        ///< Raw adu value
        double  temp;       ///< Resulting temperature
    } XMsgADUToTempData;

    /// Temperature conversion modes
    /// These constants determine what temperature unit is used to interact with the thermal filter(s)
    enum TempConvT 
    { 
        TC_Kelvin       = 0, ///< Kelvin
        TC_Celsius      = 1, ///< Degrees celsius
        TC_Fahrenheit   = 2, ///< Fahrenheit
        TC_WMM2         = 3  ///< W.m^-2
    };

    /// Filter structure used to fetch/store scene emissivity/focal parameters (XMsgGetEN, XMsgSetEN)
    typedef struct {
        int         size;           ///< sizeof(XMsgTempParms)
        double      e;              ///< Scene emissivity               (1-100)
        double      n;              ///< Focal/aperture parameter       (1->)
        double      ambient;        ///< Ambient/Background Temperature (Unit: chosen at filter creation)
        double      atmost;         ///< Atmosphere temperature
        double      atmostrans;     ///< Optical transmission thru the atmosphere

        TempConvT   unit;           ///< Active thermal conversion unit (read only, specified at filter instantiation)
    } XMsgTempParms;

    /// Filter structure used to transmit adu offset and gain correction to the thermal filter.
    typedef struct {
        int         size;           ///< sizeof(XMsgTempAdjustmentParms)
        double      offset;         ///< linear transform (emicoradu*gain)+offset
        double      gain;           ///< linear transform
    } XMsgTempAdjustmentParms;

    /// XMsgTempParms Example: 
    ///
    /// XMsgTempParms temp;
    /// 
    /// XC_MsgImageFilter(handle, thermalfilterid, XMsgGetEN, &temp);
    /// temp.ambient = 27.5;
    /// XC_MsgImageFilter(handle, thermalfilterid, XMsgSetEN, &temp);

    /// Set temperature region of interest in pct
    typedef struct {
        double pctlo;       ///< Percentage of range, low point
        double pcthi;       ///< Percentage of range, high point
    } XMsgSetTROIParms;

    /// Push a new thermal linearization range to the thermal filter
    typedef struct {
        int    size;        ///< sizeof(XMsgRangeUpdate)
        double tmin;        ///< new minimum
        double tmax;        ///< new maximum
        int    reference;   ///< in/out reference to keep track of the update
        bool   add;         ///< true = new range request, false, remove reference from the range expansion queue.
    } XMsgRangeUpdate;

    /// Request the freeze filter to save it's data.
    typedef struct {
        int             size;           ///< sizeof(XMsgFltSaveData)
        char            path[512];      ///< Zero terminated absolute path
        unsigned long   flags;          ///< Save options (\sa XSaveDataFlags)
    } XMsgFltSaveData;


    /// Request the NUC filter's exposure time limits.
    typedef struct
    {
        int size;                       ///< sizeof(XMsgFltGetRange)
        double lo;                      ///< Low
        double hi;                      ///< Hi
    } XMsgGetRange;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Filter utility functions, C++
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#   ifdef __cplusplus
    // Some MsgImageFilter wrappers

    /// Retrieves temperature filter corrected data (used by Xeneth internally)
    /// \param cam Camera handle
    /// \param fid Filter ID
    /// \param x X-Coordinate
    /// \param y Y-Coordinate
    /// \param e Emisivity
    double IMPEXP XFltGetTValue                    (XCamera *cam, FilterID fid, double e, dword x, dword y);

    /// Deprecated function, do not use.
    double IMPEXP XFltGetValue                     (XCamera *cam, FilterID fid, dword x, dword y);

    /// Convert an ADU value to a temperature (used by GTS!)
    /// \param cam      Camera handle
    /// \param fid      Filter ID of the temperature correction filter
    /// \param adu      Input ADU value
    /// \param temp     Output Temperature Value
    ErrCode IMPEXP XFltADUToTemperature            (XCamera *cam, FilterID fid, dword adu, double *temp);

    /// Convert a temperature value to an ADU
    /// \param cam      Camera handle
    /// \param fid      Filter ID of the temperature correction filter
    /// \param adu      Output ADU value
    /// \param temp     Input Temperature Value
    ErrCode IMPEXP XFltTemperatureToADU            (XCamera *cam, FilterID fid, double temp, dword *adu);

    /// Convert a linearized ADU value to temperature (special option of the thermal filters)
    /// \param cam      Camera handle
    /// \param fid      Filter ID of the temperature correction filter
    /// \param adu      ADU value
    /// \param temp     Temperature Value
    ErrCode IMPEXP XFltADUToTemperatureLin         (XCamera *cam, FilterID fid, dword adu, double *temp);

    /// Convert a temperature value to a linearized ADU (special option of the thermal filters)
    /// \param cam      Camera handle
    /// \param fid      Filter ID of the temperature correction filter
    /// \param adu      ADU value
    /// \param temp     Temperature Value
    ErrCode IMPEXP XFltTemperatureToADULin         (XCamera *cam, FilterID fid, double temp, dword *adu);

    /// Sends a stream to an image filter.
    /// \param cam      Camera handle
    /// \param fid      Filter ID
    /// \param msg      Message \sa XFilterMessage
    /// \param p        Buffer pointer
    /// \param len      Length to transmit in bytes
    ErrCode IMPEXP XFltSendStream                  (XCamera *cam, FilterID fid, XFilterMessage msg, const char *p, int len);

    /// Receives a stream from an image filter.
    /// \param cam      Camera handle
    /// \param fid      Filter ID
    /// \param msg      Message \sa XFilterMessage
    /// \param p        Buffer pointer
    /// \param len      Length to transmit in bytes
    ErrCode IMPEXP XFltRecvStream                  (XCamera *cam, FilterID fid, XFilterMessage msg, char *p, int *len);

    /// Simplified access to filter parameters
    /// \param cam      Camera handle
    /// \param fid      Filter ID
    /// \param parm     Filter property name (use "XML" to transmit an entire XML structure to a filter)
    /// \param value    Zero terminated string
    ErrCode IMPEXP XFltSetParameter                (XCamera *cam, FilterID fid, const char *parm, const char *value);

    /// Simplified access to filter parameters
    /// \param cam      Camera handle
    /// \param fid      Filter ID
    /// \param parm     Filter property name (use "XML" to transmit an entire XML structure to a filter)
    /// \param value    Output buffer
    /// \param len      in: Space reserved in output buffer out: Actual length
    ErrCode IMPEXP XFltGetParameter                (XCamera *cam, FilterID fid, const char *parm, char *value, int *len);

#endif
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Filter utility functions, C
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#   ifdef __cplusplus
    extern "C" 
    {
#   endif

        /// Retrieves temperature filter corrected data (used by Xeneth internally)
        /// \param hnd Camera handle
        /// \param fid Filter ID
        /// \param x X-Coordinate
        /// \param y Y-Coordinate
        /// \param e Emisivity
        double IMPEXPC XC_FLT_GetTValue                     (XCHANDLE hnd, FilterID fid, double e, dword x, dword y);

        /// Deprecated function, do not use.
        double IMPEXPC XC_FLT_GetValue                      (XCHANDLE hnd, FilterID fid, dword x, dword y);

        /// Convert an ADU value to a temperature (used by GTS!)
        /// \param hnd      Camera handle
        /// \param fid      Filter ID of the temperature correction filter
        /// \param adu      Input ADU value
        /// \param temp     Output Temperature Value
        ErrCode IMPEXPC XC_FLT_ADUToTemperature             (XCHANDLE hnd, FilterID fid, dword adu, double *temp);

        /// Convert a temperature value to an ADU
        /// \param hnd      Camera handle
        /// \param fid      Filter ID of the temperature correction filter
        /// \param adu      Output ADU value
        /// \param temp     Input Temperature Value
        ErrCode IMPEXPC XC_FLT_TemperatureToADU             (XCHANDLE hnd, FilterID fid, double temp, dword *adu);

        /// Convert a linearized ADU value to temperature (special option of the thermal filters)
        /// \param hnd      Camera handle
        /// \param fid      Filter ID of the temperature correction filter
        /// \param adu      ADU value
        /// \param temp     Temperature Value
        ErrCode IMPEXPC XC_FLT_ADUToTemperatureLin          (XCHANDLE hnd, FilterID fid, dword adu, double *temp);

        /// Convert a temperature value to a linearized ADU (special option of the thermal filters)
        /// \param hnd      Camera handle
        /// \param fid      Filter ID of the temperature correction filter
        /// \param adu      ADU value
        /// \param temp     Temperature Value
        ErrCode IMPEXPC XC_FLT_TemperatureToADULin          (XCHANDLE hnd, FilterID fid, double temp, dword *adu);

        /// Sends a stream to an image filter.
        /// \param hnd      Camera handle
        /// \param fid      Filter ID
        /// \param msg      Message \sa XFilterMessage
        /// \param p        Buffer pointer
        /// \param len      Length to transmit in bytes
        ErrCode IMPEXPC XC_FLT_SendStream                   (XCHANDLE hnd, FilterID fid, XFilterMessage msg, const char *p, int len);

        /// Receives a stream from an image filter.
        /// \param hnd      Camera handle
        /// \param fid      Filter ID
        /// \param msg      Message \sa XFilterMessage
        /// \param p        Buffer pointer
        /// \param len      Length to transmit in bytes
        ErrCode IMPEXPC XC_FLT_RecvStream                   (XCHANDLE hnd, FilterID fid, XFilterMessage msg, char *p, int *len);

        /// Simplified access to filter parameters
        /// \param hnd      Camera handle
        /// \param fid      Filter ID
        /// \param parm     Filter property name (use "XML" to transmit an entire XML structure to a filter)
        /// \param value    Zero terminated string
        ErrCode IMPEXPC XC_FLT_SetParameter                 (XCHANDLE hnd, FilterID fid, const char *parm, const char *value);

        /// Simplified access to filter parameters
        /// \param hnd      Camera handle
        /// \param fid      Filter ID
        /// \param parm     Filter property name (use "XML" to fetch an entire XML structure from a filter)
        /// \param value    Output buffer
        /// \param len      in: Space reserved in output buffer out: Actual length
        ErrCode IMPEXPC XC_FLT_GetParameter                 (XCHANDLE hnd, FilterID fid, const char *parm, char *value, int *len);

#   ifdef __cplusplus
    }
#   endif


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // User plug in development functions
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#   ifdef __cplusplus
    extern "C" 
    {
#   endif

        /// Filter interface version number.
#       define _FLT_IFV (43)

        /// Convert a Xeneth API stream to a C text string
        /**
        <h3>Filter serialisation structure</h3>
        This function is used in conjunction with the XMsgSerialise and XMsgDeserialise filter messages.<br><br>
        &lt;XFilter&gt;<br>
        &lt;version&gt;versionid&lt;/version&gt;<br>
        &lt;propertyname1 type='string' label='PropertyLabel' range='' editable='1'&gt;propertyvalue&lt;/propertyname1&gt;<br>
        &lt;propertyname2 type='int'    label='PropertyLabel' range='' editable='1'&gt;propertyvalue&lt;/propertyname2&gt;<br>
        &lt;propertyname3 type='float'  label='PropertyLabel' range='' editable='1'&gt;propertyvalue&lt;/propertyname3&gt;<br>
        &lt;propertyname4 type='enum'   label='PropertyLabel' range='choicea,choiceb,choicec' editable='1'&gt;choicea&lt;/propertyname4&gt;<br>
        &lt;propertyname5 type='bool'   label='PropertyLabel' range='' editable='1'&gt;True/propertyname5&gt;<br>
        &lt;propertyname6 type='colour' label='PropertyLabel' range='' editable='1'&gt;rgb(255,0,0)&lt;/propertyname6&gt;<br>
        &lt;propertyname7 type='matrix' label='PropertyLabel' range='' editable='1'&gt;1;2;3;4;5;6;7;8;9&lt;/propertyname7&gt;<br>
        &lt;propertyname8 type='file'   label='PropertyLabel' range='' editable='1'&gt;propertyvalue&lt;/propertyname8&gt;<br>
        &lt;/XFilter&gt;
        <h3>Attributes:</h3>
        type  - datatype (string,int,float,enum,bool,colour,matrix,file)<br>
        label - displayed name in the user interface<br>
        range - possible values<br>
        editable - 0 = readonly / 1 = editable<br>
        */
        /// \param pStream  - Stream pointer received by handling XMsgDeserialise
        /// \param pText    - Output -> zero terminated string
        /// \param len      - in: Space reserved in output buffer out: Actual length
        ErrCode  IMPEXPC XC_FLT_StreamToText                (void *pStream, char *pText, int *len);

        /// Convert a Xeneth API stream to a C text string
        /// \param pStream  - Stream pointer received by handling XMsgSerialise
        /// \param pText    - Zero terminated C string
        /// \param len      - Length of the string
        ErrCode  IMPEXPC XC_FLT_TextToStream                (void *pStream, const char *pText, int len);

#   ifdef __cplusplus
    }
#   endif

#endif
