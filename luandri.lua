ffi = require("ffi")
C = ffi.C
local luandri = ffi.load("./libluandri.so")

ffi.cdef[[
    typedef struct QueryRequest QueryRequest;
    QueryRequest* QueryRequest_newFromParams(const char* query, const char** formulators, int formulatorsCount, const char** metadata, int metadataCount, const int* docSet, int docSetCount, int resultsRequested, int startNum, int options);
    QueryRequest* QueryRequest_newNoParams();
    void QueryRequest_delete(QueryRequest* qr);
    char* QueryRequest_getQuery(const QueryRequest* qr);
    int QueryRequest_getFormulatorsCount(const QueryRequest* qr);
    char** QueryRequest_getFormulators(const QueryRequest* qr);
    int QueryRequest_getMetadataCount(const QueryRequest* qr);
    char** QueryRequest_getMetadata(const QueryRequest* qr);
    int QueryRequest_getDocSetCount(const QueryRequest* qr);
    int* QueryRequest_getDocSet(const QueryRequest* qr);
    int QueryRequest_getResultsRequested(const QueryRequest* qr);
    int QueryRequest_getStartNum(const QueryRequest* qr);
    int QueryRequest_getOptions(const QueryRequest* qr);
    void QueryRequest_setQuery(QueryRequest* qr, const char* query);
    void QueryRequest_setFormulators(QueryRequest* qr, const char** formulators, int formulatorsCount);
    void QueryRequest_setMetadata(QueryRequest* qr, const char** metadata, int metadataCount);
    void QueryRequest_setDocSet(QueryRequest* qr, const int* docSet, int docSetCount);
    void QueryRequest_setResultsRequested(QueryRequest* qr, int resultsRequested);
    void QueryRequest_setStartNum(QueryRequest* qr, int startNum);
    void QueryRequest_setOptions(QueryRequest* qr, int options);

    typedef struct MetadataPair MetadataPair;
    MetadataPair* MetadataPair_newFromParams(const char* key, const char* value);
    MetadataPair* MetadataPair_newNoParams();
    void MetadataPair_delete(MetadataPair* mp);
    char* MetadataPair_getKey(const MetadataPair* mp);
    char* MetadataPair_getValue(const MetadataPair* mp);
    void MetadataPair_setKey(MetadataPair* mp, const char* key);
    void MetadataPair_setValue(MetadataPair* mp, const char* value);

    typedef struct QueryResult QueryResult;
    QueryResult* QueryResult_newFromParams(const char* snippet, const char* documentName, int docid, double score, int begin, int end, const intptr_t* metadata, int metadataCount);
    QueryResult* QueryResult_newNoParams();
    void QueryResult_delete(QueryResult* qr);
    char* QueryResult_getSnippet(const QueryResult* qr);
    char* QueryResult_getDocumentName(const QueryResult* qr);
    int QueryResult_getDocid(QueryResult* qr);
    double QueryResult_getScore(QueryResult* qr);
    int QueryResult_getBegin(QueryResult* qr);
    int QueryResult_getEnd(QueryResult* qr);
    int QueryResult_getMetadataCount(QueryResult* qr);
    MetadataPair** QueryResult_getMetadata(QueryResult* qr);
    void QueryResult_setSnippet(QueryResult* qr, const char* snippet);
    void QueryResult_setDocumentName(QueryResult* qr, const char* documentName);
    void QueryResult_setDocid(QueryResult* qr, int docid);
    void QueryResult_setScore(QueryResult* qr, double score);
    void QueryResult_setBegin(QueryResult* qr, int begin);
    void QueryResult_setEnd(QueryResult* qr, int end);
    void QueryResult_setMetadata(QueryResult* qr, MetadataPair* metadata, int metadataCount);

    typedef struct QueryResults QueryResults;
    QueryResults* QueryResults_newFromParams(float parseTime, float executeTime, float documentsTime, int estimatedMatches, const intptr_t* results, int resultsCount);
    QueryResults* QueryResults_newNoParams();
    void QueryResults_delete(QueryResults* qr);
    float QueryResults_getParseTime(const QueryResults* qr);
    float QueryResults_getExecuteTime(const QueryResults* qr);
    float QueryResults_getDocumentsTime(const QueryResults* qr);
    int QueryResults_getEstimatedMatches(const QueryResults* qr);
    int QueryResults_getResultsCount(const QueryResults* qr);
    QueryResult** QueryResults_getResults(const QueryResults* qr);
    void QueryResults_setParseTime(QueryResults* qr, float parseTime);
    void QueryResults_setExecuteTime(QueryResults* qr, float executeTime);
    void QueryResults_setDocumentsTime(QueryResults* qr, float documentsTime);
    void QueryResults_setEstimatedMatches(QueryResults* qr, int estimatedMatches);
    void QueryResults_setResults(QueryResults* qr, const intptr_t* results, int resultsCount);

    typedef struct QueryEnvironment QueryEnvironment;
    QueryEnvironment* QueryEnvironment_newNoParams();
    void QueryEnvironment_delete(QueryEnvironment* qe);
    void QueryEnvironment_setMemory(QueryEnvironment* qe, uint64_t memory);
    void QueryEnvironment_setStopwords(QueryEnvironment* qe, const char** stopwords, int stopwordsCount);
    void QueryEnvironment_addIndex(QueryEnvironment* qe, const char* pathname);
    QueryResults* QueryEnvironment_runQuery(QueryEnvironment* qe, QueryRequest *request);

    /* From the C library */
    void free(void*);
]]


-- Helper methods --

function table2Ptr(tbl, elemtype)
    local intrm
    if elemtype == "string" then
        intrm = {}
        for k, v in pairs(tbl) do
            local s = ffi.new("char[?]", #v)
            ffi.copy(s, v)
            intrm[k] = s
        end
        elemtype = "char*"
    elseif elemtype == "intptr_t" then
        intrm = {}
        for k, v in pairs(tbl) do
            intrm[k] = ffi.cast("intptr_t", v.cdata)
        end
    else
        intrm = tbl
    end

    local arr = ffi.new(elemtype .. "[" .. #intrm .. "]", intrm)
    local ptr = ffi.cast("const " .. elemtype .. "*", arr)
    return ptr
end

function ptr2table(ptr, elemtype, count, wrapper, deletefunc)
    local tbl = {}
    ffi.gc(ptr, C.free)
    for i= 0, count-1 do
        if elemtype == "string" then
            ffi.gc(ptr[i], C.free)
            tbl[i + 1] = ffi.string(ptr[i])
        elseif elemtype == "cdata" then
            local self = {cdata = ptr[i]}
            ffi.gc(self.cdata, deletefunc)
            tbl[i + 1] = setmetatable(self, wrapper)
        else
            tbl[i + 1] = ptr[i]
        end
    end

    return tbl
end

-- QueryRequestWrapper --

local QueryRequestWrapper = {}
QueryRequestWrapper.__index = QueryRequestWrapper
QueryRequestOptions = { HTMLSnippet = 1, TextSnippet = 2 }

function QueryRequestFromParams(query, formulators, metadata, docSet, resultsRequested, startNum, options)
    return QueryRequestFromCData(luandri.QueryRequest_newFromParams(query, table2Ptr(formulators, "string"), #formulators, table2Ptr(metadata, "string"), #metadata, table2Ptr(docSet, "int"), #docSet, resultsRequested, startNum, options))
end

function QueryRequestNoParams()
    return QueryRequestFromCData(luandri.QueryRequest_newNoParams())
end

function QueryRequestFromCData(inp_cdata)
    local self = {cdata = inp_cdata}
    ffi.gc(self.cdata, luandri.QueryRequest_delete)
    return setmetatable(self, QueryRequestWrapper)
end

function QueryRequestFromTable(tbl)
    local obj = QueryRequestNoParams()
    if tbl.query ~= nil then
        obj:setQuery(tbl.query)
    end
    if tbl.formulators ~= nil then
        obj:setFormulators(tbl.formulators)
    end
    if tbl.metadata ~= nil then
        obj:setMetadata(tbl.metadata)
    end
    if tbl.docSet ~= nil then
        obj:setDocSet(tbl.docSet)
    end
    if tbl.resultsRequested ~= nil then
        obj:setResultsRequested(tbl.resultsRequested)
    end
    if tbl.startNum ~= nil then
        obj:setStartNum(tbl.startNum)
    end
    if tbl.options ~= nil then
        obj:setOptions(tbl.options)
    end

    return obj
end

function QueryRequestWrapper.asTable(self)
    local tbl = {}
    tbl.query = self:getQuery()
    tbl.formulators = self:getFormulators()
    tbl.metadata = self:getMetadata()
    tbl.docSet = self:getDocSet()
    tbl.resultsRequested = self:getResultsRequested()
    tbl.startNum = self:getStartNum()
    tbl.options = self:getOptions()

    return tbl
end

function QueryRequestWrapper.getQuery(self)
    local query = luandri.QueryRequest_getQuery(self.cdata)
    ffi.gc(query, C.free)
    return ffi.string(query)
end

function QueryRequestWrapper.getFormulators(self)
    local count = luandri.QueryRequest_getFormulatorsCount(self.cdata)
    local arr = luandri.QueryRequest_getFormulators(self.cdata)
    return ptr2table(arr, "string", count)
end

function QueryRequestWrapper.getMetadata(self)
    local count = luandri.QueryRequest_getMetadataCount(self.cdata)
    local arr = luandri.QueryRequest_getMetadata(self.cdata)
    return ptr2table(arr, "string", count)
end

function QueryRequestWrapper.getDocSet(self)
    local count = luandri.QueryRequest_getDocSetCount(self.cdata)
    local arr = luandri.QueryRequest_getDocSet(self.cdata)
    return ptr2table(arr, "int", count)
end

function QueryRequestWrapper.getResultsRequested(self)
    return luandri.QueryRequest_getResultsRequested(self.cdata)
end

function QueryRequestWrapper.getStartNum(self)
    return luandri.QueryRequest_getStartNum(self.cdata)
end

function QueryRequestWrapper.getOptions(self)
    return luandri.QueryRequest_getOptions(self.cdata)
end

function QueryRequestWrapper.setQuery(self, query)
    luandri.QueryRequest_setQuery(self.cdata, query)
end

function QueryRequestWrapper.setFormulators(self, formulators)
    luandri.QueryRequest_setFormulators(self.cdata, table2Ptr(formulators, "string"), #formulators)
end

function QueryRequestWrapper.setMetadata(self, metadata)
    luandri.QueryRequest_setMetadata(self.cdata, table2Ptr(metadata, "string"), #metadata)
end

function QueryRequestWrapper.setDocSet(self, docSet)
    luandri.QueryRequest_setDocSet(self.cdata, table2Ptr(docSet, "int"), #docSet)
end

function QueryRequestWrapper.setResultsRequested(self, resultsRequested)
    luandri.QueryRequest_setResultsRequested(self.cdata, resultsRequested)
end

function QueryRequestWrapper.setStartNum(self, startNum)
    luandri.QueryRequest_setStartNum(self.cdata, startNum)
end

function QueryRequestWrapper.setOptions(self, options)
    luandri.QueryRequest_setOptions(self.cdata, options)
end


-- MetadataPairWrapper --

local MetadataPairWrapper = {}
MetadataPairWrapper.__index = MetadataPairWrapper

function MetadataPairFromParams(key, value)
    return MetadataPairFromCData(luandri.MetadataPair_newFromParams(key, value))
end

function MetadataPairNoParams()
    return MetadataPairFromCData(luandri.MetadataPair_newNoParams())
end

function MetadataPairFromCData(inp_cdata)
    local self = {cdata = inp_cdata}
    ffi.gc(self.cdata, luandri.MetadataPair_delete)
    return setmetatable(self, MetadataPairWrapper)
end

function MetadataPairFromTable(tbl)
    local obj = MetadataPairNoParams()
    if tbl.key ~= nil then
        obj:setKey(tbl.key)
    end
    if tbl.value ~= nil then
        obj:setValue(tbl.value)
    end

    return obj
end

function MetadataPairWrapper.asTable(self)
    local tbl = {}
    tbl.key = self:getKey()
    tbl.value = self:getValue()

    return tbl
end

function MetadataPairWrapper.getKey(self)
    local key = luandri.MetadataPair_getKey(self.cdata)
    ffi.gc(key, C.free)
    return ffi.string(key)
end

function MetadataPairWrapper.getValue(self)
    local value = luandri.MetadataPair_getValue(self.cdata)
    ffi.gc(value, C.free)
    return ffi.string(value)
end

function MetadataPairWrapper.setKey(self, key)
    luandri.MetadataPair_setKey(self.cdata, key)
end

function MetadataPairWrapper.setValue(self, value)
    luandri.MetadataPair_setValue(self.cdata, value)
end


-- QueryResultWrapper --

local QueryResultWrapper = {}
QueryResultWrapper.__index = QueryResultWrapper

function QueryResultFromParams(snippet, documentName, docid, score, begining, ending, metadata)
    return QueryResultFromCData(luandri.QueryResult_newFromParams(snippet, documentName, docid, score, begining, ending, table2Ptr(metadata, "intptr_t"), #metadata))
end

function QueryResultNoParams()
    return QueryResultFromCData(luandri.QueryResult_newNoParams())
end

function QueryResultFromCData(inp_cdata)
    local self = {cdata = inp_cdata}
    ffi.gc(self.cdata, luandri.QueryResult_delete)
    return setmetatable(self, QueryResultWrapper)
end

function QueryResultFromTable(tbl)
    local obj = QueryResultNoParams()
    if tbl.snippet ~= nil then
        obj:setSnippet(tbl.snippet)
    end
    if tbl.documentName ~= nil then
        obj:setDocumentName(tbl.documentName)
    end
    if tbl.docid ~= nil then
        obj:setDocid(tbl.docid)
    end
    if tbl.score ~= nil then
        obj:setScore(tbl.score)
    end
    if tbl.beginning ~= nil then
        obj:setBeginning(tbl.beginning)
    end
    if tbl.ending ~= nil then
        obj:setEnding(tbl.ending)
    end
    if tbl.metadata ~= nil then
        obj:setMetadata(tbl.metadata)
    end

    return obj
end

function QueryResultWrapper.asTable(self)
    local tbl = {}
    tbl.snippet = self:getSnippet()
    tbl.documentName = self:getDocumentName()
    tbl.docid = self:getDocid()
    tbl.score = self:getScore()
    tbl.beginning = self:getBegin()
    tbl.ending = self:getEnd()
    tbl.metadata = self:getMetadata()

    return tbl
end

function QueryResultWrapper.getSnippet(self)
    local snippet = luandri.QueryResult_getSnippet(self.cdata)
    ffi.gc(snippet, C.free)
    return ffi.string(snippet)
end

function QueryResultWrapper.getDocumentName(self)
    local documentName = luandri.QueryResult_getDocumentName(self.cdata)
    ffi.gc(documentName, C.free)
    return ffi.string(documentName)
end

function QueryResultWrapper.getDocid(self)
    local docid = luandri.QueryResult_getDocid(self.cdata)
    return docid
end

function QueryResultWrapper.getScore(self)
    local score = luandri.QueryResult_getScore(self.cdata)
    return score
end

function QueryResultWrapper.getBegin(self)
    local beginning = luandri.QueryResult_getBegin(self.cdata)
    return beginning
end

function QueryResultWrapper.getEnd(self)
    local ending = luandri.QueryResult_getEnd(self.cdata)
    return ending
end

function QueryResultWrapper.getMetadata(self)
    local count = luandri.QueryResult_getMetadataCount(self.cdata)
    local arr = luandri.QueryResult_getMetadata(self.cdata)
    return ptr2table(arr, "cdata", count, MetadataPairWrapper, luandri.MetadataPair_delete)
end

function QueryResultWrapper.setSnippet(self, snippet)
    luandri.QueryResult_setSnippet(self.cdata, snippet)
end

function QueryResultWrapper.setDocumentName(self, documentName)
    luandri.QueryResult_setDocumentName(self.cdata, documentName)
end

function QueryResultWrapper.setDocid(self, docid)
    luandri.QueryResult_setDocid(self.cdata, docid)
end

function QueryResultWrapper.setScore(self, score)
    luandri.QueryResult_setScore(self.cdata, score)
end

function QueryResultWrapper.setBegin(self, beginning)
    luandri.QueryResult_setBegin(self.cdata, beginning)
end

function QueryResultWrapper.setEnd(self, ending)
    luandri.QueryResult_setEnd(self.cdata, ending)
end

function QueryResultWrapper.setMetadata(self, metadata)
    luandri.QueryResult_setMetadata(self.cdata, table2Ptr(metadata, "intptr_t"), #metadata)
end


-- QueryResultWrappers --

local QueryResultsWrapper = {}
QueryResultsWrapper.__index = QueryResultsWrapper

function QueryResultsFromParams(parseTime, executeTime, documentsTime, estimatedMatches, results)
    return QueryResultsFromCData(luandri.QueryResults_newFromParams(parseTime, executeTime, documentsTime, estimatedMatches, table2Ptr(results, "intptr_t"), #results))
end

function QueryResultsNoParams()
    return QueryResultsFromCData(luandri.QueryResults_newNoParams())
end

function QueryResultsFromCData(inp_cdata)
    local self = {cdata = inp_cdata}
    ffi.gc(self.cdata, luandri.QueryResults_delete)
    return setmetatable(self, QueryResultsWrapper)
end

function QueryResultsFromTable(tbl)
    local obj = QueryResultsNoParams()
    if tbl.parseTime ~= nil then
        obj:setParseTime(tbl.parseTime)
    end
    if tbl.executeTime ~= nil then
        obj:setExecuteTime(tbl.executeTime)
    end
    if tbl.documentsTime ~= nil then
        obj:setDocumentsTime(tbl.documentsTime)
    end
    if tbl.estimatedMatches ~= nil then
        obj:setEstimatedMatches(tbl.estimatedMatches)
    end
    if tbl.results ~= nil then
        obj:setResults(tbl.results)
    end

    return obj
end

function QueryResultsWrapper.asTable(self)
    local tbl = {}
    tbl.parseTime = self:getParseTime()
    tbl.executeTime = self:getExecuteTime()
    tbl.documentsTime = self:getDocumentsTime()
    tbl.estimatedMatches = self:getEstimatedMatches()
    tbl.results = {}
    cResults = self:getResults()
    for k, v in pairs(cResults) do
        tbl.results[k] = v:asTable()
    end

    return tbl
end

function QueryResultsWrapper.getParseTime(self)
    local parseTime = luandri.QueryResults_getParseTime(self.cdata)
    return parseTime
end

function QueryResultsWrapper.getExecuteTime(self)
    local executeTime = luandri.QueryResults_getExecuteTime(self.cdata)
    return executeTime
end

function QueryResultsWrapper.getDocumentsTime(self)
    local documentsTime = luandri.QueryResults_getDocumentsTime(self.cdata)
    return documentsTime
end

function QueryResultsWrapper.getEstimatedMatches(self)
    local estimatedMatches = luandri.QueryResults_getEstimatedMatches(self.cdata)
    return estimatedMatches
end

function QueryResultsWrapper.getResults(self)
    local count = luandri.QueryResults_getResultsCount(self.cdata)
    local arr = luandri.QueryResults_getResults(self.cdata)
    return ptr2table(arr, "cdata", count, QueryResultWrapper, luandri.QueryResult_delete)
end

function QueryResultsWrapper.setParseTime(self, parseTime)
    luandri.QueryResults_setParseTime(self.cdata, parseTime)
end

function QueryResultsWrapper.setExecuteTime(self, executeTime)
    luandri.QueryResults_setExecuteTime(self.cdata, executeTime)
end

function QueryResultsWrapper.setDocumentsTime(self, documentsTime)
    luandri.QueryResults_setDocumentsTime(self.cdata, documentsTime)
end

function QueryResultsWrapper.setEstimatedMatches(self, estimatedMatches)
    luandri.QueryResults_setEstimatedMatches(self.cdata, estimatedMatches)
end

function QueryResultsWrapper.setResults(self, results)
    luandri.QueryResults_setResults(self.cdata, table2Ptr(results, "intptr_t"), #results)
end


-- QueryEnvironmentWrapper --

local QueryEnvironmentWrapper = {}
QueryEnvironmentWrapper.__index = QueryEnvironmentWrapper

function QueryEnvironmentNoParams()
    return QueryEnvironmentFromCData(luandri.QueryEnvironment_newNoParams())
end

function QueryEnvironmentFromCData(inp_cdata)
    local self = {cdata = inp_cdata}
    ffi.gc(self.cdata, luandri.QueryEnvironment_delete)
    return setmetatable(self, QueryEnvironmentWrapper)
end

function QueryEnvironmentWrapper.setMemory(self, memory)
    luandri.QueryEnvironment_setMemory(self.cdata, memory)
end

function QueryEnvironmentWrapper.setStopwords(self, stopwords)
    luandri.QueryEnvironment_setStopwords(self.cdata, table2Ptr(stopwords, "string"), #stopwords)
end

function QueryEnvironmentWrapper.addIndex(self, pathname)
    luandri.QueryEnvironment_addIndex(self.cdata, pathname)
end

function QueryEnvironmentWrapper.runQuery(self, request)
    return luandri.QueryEnvironment_runQuery(self.cdata, request.cdata)
end


-- QueryEnvironmentLua --

local QueryEnvironmentLua = {}
QueryEnvironmentLua.__index = QueryEnvironmentLua

function QueryEnvironment()
    local self = {QueryEnvironmentNative = QueryEnvironmentNoParams()}
    return setmetatable(self, QueryEnvironmentLua)
end

function QueryEnvironmentLua.setMemory(self, memory)
    self.QueryEnvironmentNative:setMemory(memory)
end

function QueryEnvironmentLua.setStopwords(self, stopwords)
    self.QueryEnvironmentNative:setStopwords(stopwords)
end

function QueryEnvironmentLua.addIndex(self, pathname)
    self.QueryEnvironmentNative:addIndex(pathname)
end

function QueryEnvironmentLua.runQuery(self, request)
    local cRequest = QueryRequestFromTable(request)
    local cResponse = self.QueryEnvironmentNative:runQuery(cRequest)
    local response = QueryResultsFromCData(cResponse)
    return response:asTable()
end