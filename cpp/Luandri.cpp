#include "indri/QueryEnvironment.hpp"
#include <time.h>

std::vector<std::string> StringVectorFromPointer(const char** base, int count)
{
    std::vector<std::string>* tmp = new std::vector<std::string>();

    for (int i = 0; i < count; i++)
    {
        tmp->push_back(std::string(*((char**)base + i)));
    }

    return *tmp;
}

/**************************************************
 Wrapper for QueryRequest
 **************************************************/

extern "C" void* QueryRequest_newFromParams(const char* query,
    const char** formulators,
    int formulatorsCount,
    const char** metadata,
    int metadataCount,
    const int* docSet,
    int docSetCount,
    int resultsRequested,
    int startNum,
    int options)
{
    assert(query != NULL);
    indri::api::QueryRequest *qr = new indri::api::QueryRequest();
    qr->query = std::string(query);
    qr->formulators = StringVectorFromPointer(formulators, formulatorsCount);
    qr->metadata = StringVectorFromPointer(metadata, metadataCount);
    qr->docSet.assign((lemur::api::DOCID_T*)docSet, (lemur::api::DOCID_T*)docSet + docSetCount);
    qr->resultsRequested = resultsRequested;
    qr->startNum = startNum;
    qr->options = (indri::api::QueryRequest::Options)options;

    return reinterpret_cast<void*>(qr);
}

extern "C" void* QueryRequest_newNoParams()
{
    indri::api::QueryRequest *qr = new indri::api::QueryRequest();
    return reinterpret_cast<void*>(qr);
}

extern "C" void     QueryRequest_delete                 (indri::api::QueryRequest* qr)                                                  { delete qr; }
extern "C" char*    QueryRequest_getQuery               (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return strdup(qr->query.c_str()); }
extern "C" int      QueryRequest_getFormulatorsCount    (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return qr->formulators.size(); }
extern "C" char**   QueryRequest_getFormulators         (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); int n = qr->formulators.size(); char** formulators = new char*[n]; for (int i = 0; i < n; i++) formulators[i] = strdup(qr->formulators[i].c_str()); return formulators; }
extern "C" int      QueryRequest_getMetadataCount       (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return qr->metadata.size(); }
extern "C" char**   QueryRequest_getMetadata            (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); int n = qr->metadata.size(); char** metadata = new char*[n]; for (int i = 0; i < n; i++) metadata[i] = strdup(qr->metadata[i].c_str()); return metadata; }
extern "C" int      QueryRequest_getDocSetCount         (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return qr->docSet.size(); }
extern "C" int*     QueryRequest_getDocSet              (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); int n = qr->docSet.size(); int* docSet = new int[n]; for (int i = 0; i < n; i++) docSet[i] = (int)qr->docSet[i]; return docSet; }
extern "C" int      QueryRequest_getResultsRequested    (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return qr->resultsRequested; }
extern "C" int      QueryRequest_getStartNum            (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return qr->startNum; }
extern "C" int      QueryRequest_getOptions             (const indri::api::QueryRequest* qr)                                            { assert(qr != NULL); return (int)qr->options; }
extern "C" void     QueryRequest_setQuery               (indri::api::QueryRequest* qr, const char* query)                               { assert(qr != NULL && query != NULL); qr->query = std::string(query); }
extern "C" void     QueryRequest_setFormulators         (indri::api::QueryRequest* qr, const char** formulators, int formulatorsCount)  { assert(qr != NULL); qr->formulators = StringVectorFromPointer(formulators, formulatorsCount); }
extern "C" void     QueryRequest_setMetadata            (indri::api::QueryRequest* qr, const char** metadata, int metadataCount)        { assert(qr != NULL); qr->metadata = StringVectorFromPointer(metadata, metadataCount); }
extern "C" void     QueryRequest_setDocSet              (indri::api::QueryRequest* qr, const int* docSet, int docSetCount)              { assert(qr != NULL); qr->docSet.assign((lemur::api::DOCID_T*)docSet, (lemur::api::DOCID_T*)docSet + docSetCount); }
extern "C" void     QueryRequest_setResultsRequested    (indri::api::QueryRequest* qr, int resultsRequested)                            { assert(qr != NULL); qr->resultsRequested = resultsRequested; }
extern "C" void     QueryRequest_setStartNum            (indri::api::QueryRequest* qr, int startNum)                                    { assert(qr != NULL); qr->startNum = startNum; }
extern "C" void     QueryRequest_setOptions             (indri::api::QueryRequest* qr, int options)                                     { assert(qr != NULL); qr->options = (indri::api::QueryRequest::Options)options; }


/**************************************************
 Wrapper for MetadataPair
**************************************************/

extern "C" void* MetadataPair_newFromParams(const char* key, const char* value)
{
    assert(key != NULL && value != NULL);
    indri::api::MetadataPair *mp = new indri::api::MetadataPair();
    mp->key = std::string(key);
    mp->value = std::string(value);

    return reinterpret_cast<void*>(mp);
}

extern "C" void* MetadataPair_newNoParams()
{
    indri::api::MetadataPair *mp = new indri::api::MetadataPair();
    return reinterpret_cast<void*>(mp);
}

indri::api::MetadataPair* MetadataPair_deepCopy(indri::api::MetadataPair mp)
{
    return (indri::api::MetadataPair*)MetadataPair_newFromParams(mp.key.c_str(), mp.value.c_str());
}

extern "C" void     MetadataPair_delete                 (indri::api::MetadataPair* mp)                                                  { delete mp; }
extern "C" char*    MetadataPair_getKey                 (const indri::api::MetadataPair* mp)                                            { assert(mp != NULL); return strdup(mp->key.c_str()); }
extern "C" char*    MetadataPair_getValue               (const indri::api::MetadataPair* mp)                                            { assert(mp != NULL); return strdup(mp->value.c_str()); }
extern "C" void     MetadataPair_setKey                 (indri::api::MetadataPair* mp, const char* key)                                 { assert(mp != NULL && key != NULL); mp->key = std::string(key); }
extern "C" void     MetadataPair_setValue               (indri::api::MetadataPair* mp, const char* value)                               { assert(mp != NULL && value != NULL); mp->value = std::string(value); }


/**************************************************
 Wrapper for QueryResult
**************************************************/

extern "C" void* QueryResult_newFromParams(const char* snippet, const char* documentName, int docid, double score, int begin, int end, const intptr_t* metadata, int metadataCount)
{
    assert(snippet != NULL && documentName != NULL);
    indri::api::QueryResult *qr = new indri::api::QueryResult();
    qr->snippet = std::string(snippet);
    qr->documentName = std::string(documentName);
    qr->docid = (lemur::api::DOCID_T)docid;
    qr->score = score;
    qr->begin = begin;
    qr->end = end;
    qr->metadata.clear();

    if (metadata != NULL)
    {
        for (int i = 0; i < metadataCount; i++)
        {
            indri::api::MetadataPair* src = *((indri::api::MetadataPair**)metadata + i);
            indri::api::MetadataPair* dst = (indri::api::MetadataPair*)MetadataPair_deepCopy(*src);
            qr->metadata.push_back(*dst);
        }
    }

    return reinterpret_cast<void*>(qr);
}

extern "C" void* QueryResult_newNoParams()
{
    indri::api::QueryResult *qr = new indri::api::QueryResult();
    return reinterpret_cast<void*>(qr);
}

indri::api::QueryResult* QueryResult_deepCopy(indri::api::QueryResult src)
{
    indri::api::QueryResult *dst = (indri::api::QueryResult*)QueryResult_newNoParams();
    dst->snippet = src.snippet;
    dst->documentName = src.documentName;
    dst->docid = src.docid;
    dst->score = src.score;
    dst->begin = src.begin;
    dst->end = src.end;

    for (int i = 0; i < src.metadata.size(); i++)
    {
        indri::api::MetadataPair* metadata = (indri::api::MetadataPair*)MetadataPair_deepCopy(src.metadata[i]);
        dst->metadata.push_back(*metadata);
    }

    return dst;
}

extern "C" void     QueryResult_delete                  (indri::api::QueryResult* qr)                                                   { delete qr; }
extern "C" char*    QueryResult_getSnippet              (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return strdup(qr->snippet.c_str()); }
extern "C" char*    QueryResult_getDocumentName         (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return strdup(qr->documentName.c_str()); }
extern "C" int      QueryResult_getDocid                (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return (int)qr->docid; }
extern "C" double   QueryResult_getScore                (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return qr->score; }
extern "C" int      QueryResult_getBegin                (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return qr->begin; }
extern "C" int      QueryResult_getEnd                  (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return qr->end; }
extern "C" int      QueryResult_getMetadataCount        (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); return qr->metadata.size(); }
extern "C" void*    QueryResult_getMetadata             (const indri::api::QueryResult* qr)                                             { assert(qr != NULL); int n = qr->metadata.size(); indri::api::MetadataPair** metadata = new indri::api::MetadataPair*[n]; for (int i = 0; i < n; i++) { metadata[i] = new indri::api::MetadataPair(qr->metadata[i]); } return reinterpret_cast<void*>(metadata); }
extern "C" void     QueryResult_setSnippet              (indri::api::QueryResult* qr, const char* snippet)                              { assert(qr != NULL && snippet != NULL); qr->snippet = std::string(snippet); }
extern "C" void     QueryResult_setDocumentName         (indri::api::QueryResult* qr, const char* documentName)                         { assert(qr != NULL && documentName != NULL); qr->documentName = std::string(documentName); }
extern "C" void     QueryResult_setDocid                (indri::api::QueryResult* qr, int docid)                                        { assert(qr != NULL); qr->docid = (lemur::api::DOCID_T)docid; }
extern "C" void     QueryResult_setScore                (indri::api::QueryResult* qr, double score)                                     { assert(qr != NULL); qr->score = score; }
extern "C" void     QueryResult_setBegin                (indri::api::QueryResult* qr, int begin)                                        { assert(qr != NULL); qr->begin = begin; }
extern "C" void     QueryResult_setEnd                  (indri::api::QueryResult* qr, int end)                                          { assert(qr != NULL); qr->end = end; }
extern "C" void     QueryResult_setMetadata             (indri::api::QueryResult* qr, const intptr_t* metadata, int metadataCount)      { assert(qr != NULL); qr->metadata.clear(); for (int i = 0; i < metadataCount; i++) { indri::api::MetadataPair* src = *((indri::api::MetadataPair**)metadata + i); indri::api::MetadataPair dst(*src); qr->metadata.push_back(dst); }}


/**************************************************
 Wrapper for QueryResults
**************************************************/

extern "C" void* QueryResults_newFromParams(float parseTime, float executeTime, float documentsTime, int estimatedMatches, const intptr_t* results, int resultsCount)
{
    assert(snippet != NULL && documentName != NULL && results != NULL);
    indri::api::QueryResults *qr = new indri::api::QueryResults();

    qr->parseTime = parseTime;
    qr->executeTime = executeTime;
    qr->documentsTime = documentsTime;
    qr->estimatedMatches = estimatedMatches;
    qr->results.clear();

    for (int i = 0; i < resultsCount; i++)
    {
        indri::api::QueryResult* src = *((indri::api::QueryResult**)results + i);
        indri::api::QueryResult* dst = (indri::api::QueryResult*)QueryResult_deepCopy(*src);
        qr->results.push_back(*dst);
    }

    return reinterpret_cast<void*>(qr);
}

extern "C" void* QueryResults_newNoParams()
{
    indri::api::QueryResults *qr = new indri::api::QueryResults();
    return reinterpret_cast<void*>(qr);
}

indri::api::QueryResults* QueryResults_deepCopy(indri::api::QueryResults src)
{
    indri::api::QueryResults *dst = (indri::api::QueryResults*)QueryResults_newNoParams();
    dst->parseTime = src.parseTime;
    dst->executeTime = src.executeTime;
    dst->documentsTime = src.documentsTime;
    dst->estimatedMatches = src.estimatedMatches;

    for (int i = 0; i < src.results.size(); i++)
    {
        indri::api::QueryResult* result = (indri::api::QueryResult*)QueryResult_deepCopy(src.results[i]);
        dst->results.push_back(*result);
    }

    return dst;
}

extern "C" void     QueryResults_delete                 (indri::api::QueryResults* qr)                                                  { delete qr; }
extern "C" float    QueryResults_getParseTime           (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); return qr->parseTime; }
extern "C" float    QueryResults_getExecuteTime         (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); return qr->executeTime; }
extern "C" float    QueryResults_getDocumentsTime       (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); return qr->documentsTime; }
extern "C" int      QueryResults_getEstimatedMatches    (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); return qr->estimatedMatches; }
extern "C" int      QueryResults_getResultsCount        (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); return qr->results.size(); }
extern "C" void*    QueryResults_getResults             (const indri::api::QueryResults* qr)                                            { assert(qr != NULL); int n = qr->results.size(); indri::api::QueryResult** results = new indri::api::QueryResult*[n]; for (int i = 0; i < n; i++) { results[i] = QueryResult_deepCopy(qr->results[i]); } return reinterpret_cast<void*>(results); }
extern "C" void     QueryResults_setParseTime           (indri::api::QueryResults* qr, float parseTime)                                 { assert(qr != NULL); qr->parseTime = parseTime; }
extern "C" void     QueryResults_setExecuteTime         (indri::api::QueryResults* qr, float executeTime)                               { assert(qr != NULL); qr->executeTime = executeTime; }
extern "C" void     QueryResults_setDocumentsTime       (indri::api::QueryResults* qr, float documentsTime)                             { assert(qr != NULL); qr->documentsTime = documentsTime; }
extern "C" void     QueryResults_setEstimatedMatches    (indri::api::QueryResults* qr, int estimatedMatches)                            { assert(qr != NULL); qr->estimatedMatches = estimatedMatches; }
extern "C" void     QueryResults_setResults             (indri::api::QueryResults* qr, const intptr_t* results, int resultsCount)       { assert(qr != NULL); qr->results.clear(); for (int i = 0; i < resultsCount; i++) { indri::api::QueryResult* src = *((indri::api::QueryResult**)results + i); indri::api::QueryResult dst(*src); qr->results.push_back(dst); }}


/**************************************************
 Wrapper for QueryEnvironment
**************************************************/

extern "C" void* QueryEnvironment_newNoParams()
{
    indri::api::QueryEnvironment *qe = new indri::api::QueryEnvironment();
    return reinterpret_cast<void*>(qe);
}

extern "C" void     QueryEnvironment_delete             (indri::api::QueryEnvironment* qe)                                              { delete qe; }
extern "C" void     QueryEnvironment_setMemory          (indri::api::QueryEnvironment* qe, uint64_t memory)                             { assert(qe != NULL); qe->setMemory((UINT64)memory); }
//extern "C" void QueryEnvironment_setBaseline(indri::api::QueryEnvironment* qe, const std::string &baseline)                 { assert(qe != NULL);           qe->setBaseline(baseline); }
//extern "C" void QueryEnvironment_setSingleBackgroundModel(indri::api::QueryEnvironment* qe, bool background)                { assert(qe != NULL);           qe->setSingleBackgroundModel(background); }
//extern "C" void QueryEnvironment_setScoringRules(indri::api::QueryEnvironment* qe, const std::vector<std::string>& rules)   { assert(qe != NULL);           qe->setScoringRules(rules); }
extern "C" void     QueryEnvironment_setStopwords       (indri::api::QueryEnvironment* qe, const char** stopwords, int stopwordsCount)  { assert(qe != NULL); qe->setStopwords(StringVectorFromPointer(stopwords, stopwordsCount)); }
//extern "C" void QueryEnvironment_addServer(indri::api::QueryEnvironment* qe, const std::string& hostname)                   { assert(qe != NULL);           qe->addServer(hostname); }
extern "C" void     QueryEnvironment_addIndex           (indri::api::QueryEnvironment* qe, const char* pathname)                        { assert(qe != NULL); qe->addIndex(std::string(pathname)); }
//extern "C" void QueryEnvironment_addIndex(indri::api::QueryEnvironment* qe, IndexEnvironment& environment)                  { assert(qe != NULL);           qe->addIndex(environment); }
//extern "C" void QueryEnvironment_close(indri::api::QueryEnvironment* qe)                                                    { assert(qe != NULL);           qe->close(); }
//extern "C" void QueryEnvironment_removeServer(indri::api::QueryEnvironment* qe, const std::string& hostname)                { assert(qe != NULL);           qe->removeServer(hostname); }
//extern "C" void QueryEnvironment_removeIndex(indri::api::QueryEnvironment* qe, const std::string& pathname)                 { assert(qe != NULL);           qe->removeIndex(pathname); }
extern "C" void*    QueryEnvironment_runQuery           (indri::api::QueryEnvironment* qe, indri::api::QueryRequest *request)           { assert(qe != NULL && request != NULL); indri::api::QueryResults qr = qe->runQuery(*request); return reinterpret_cast<void*>(QueryResults_deepCopy(qr)); }
//extern "C" std::vector<indri::api::ScoredExtentResult> QueryEnvironment_runQuery(indri::api::QueryEnvironment* qe,
//    const std::string& query, int resultsRequested, const std::string &queryType)                                           { assert(qe != NULL); return    qe->runQuery(query, resultsRequested, queryType); }
//extern "C" std::vector<indri::api::ScoredExtentResult> QueryEnvironment_runQuery(indri::api::QueryEnvironment* qe, const std::string& query,
//    const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested, const std::string &queryType)                { assert(qe != NULL); return    qe->(query, documentSet, resultsRequested, queryType); }
//extern "C" QueryAnnotation* QueryEnvironment_runAnnotatedQuery(indri::api::QueryEnvironment* qe, const std::string& query,
//    int resultsRequested, const std::string &queryType)                                                                     { assert(qe != NULL); return    qe->runAnnotatedQuery(query, resultsRequested, queryType); }
//extern "C" QueryAnnotation* QueryEnvironment_runAnnotatedQuery(indri::api::QueryEnvironment* qe, const std::string& query,
//    const std::vector<lemur::api::DOCID_T>& documentSet, int resultsRequested, const std::string &queryType)                { assert(qe != NULL); return    qe->runAnnotatedQuery(query, documentSet, resultsRequested, queryType); }
//extern "C" std::vector<indri::api::ParsedDocument*> QueryEnvironment_documents(indri::api::QueryEnvironment* qe,
//    const std::vector<lemur::api::DOCID_T>& documentIDs)                                                                    { assert(qe != NULL); return    qe->documents(documentIDs); }
//extern "C" std::vector<indri::api::ParsedDocument*> QueryEnvironment_documents(indri::api::QueryEnvironment* qe,
//    const std::vector<indri::api::ScoredExtentResult>& results)                                                             { assert(qe != NULL); return    qe->documents(results); }
//extern "C" std::vector<std::string> QueryEnvironment_documentMetadata(indri::api::QueryEnvironment* qe,
//    const std::vector<lemur::api::DOCID_T>& documentIDs, const std::string& attributeName)                                  { assert(qe != NULL); return    qe->documentMetadata(documentIDs, attributeName); }
//extern "C" std::vector<std::string> QueryEnvironment_documentMetadata(indri::api::QueryEnvironment* qe,
//    const std::vector<indri::api::ScoredExtentResult>& documentIDs, const std::string& attributeName)                       { assert(qe != NULL); return    qe->documentMetadata(documentIDs, attributeName); }
//extern "C" std::vector<std::string> QueryEnvironment_pathNames(indri::api::QueryEnvironment* qe,
//    const std::vector<indri::api::ScoredExtentResult>& results)                                                             { assert(qe != NULL); return    qe->pathNames(results); }
//extern "C" std::vector<indri::api::ParsedDocument*> QueryEnvironment_documentsFromMetadata(indri::api::QueryEnvironment* qe,
//    const std::string& attributeName, const std::vector<std::string>& attributeValues)                                      { assert(qe != NULL); return    qe->documentsFromMetadata(attributeName, attributeValues); }
//extern "C" std::vector<lemur::api::DOCID_T> QueryEnvironment_documentIDsFromMetadata(indri::api::QueryEnvironment* qe,
//    const std::string& attributeName, const std::vector<std::string>& attributeValue)                                       { assert(qe != NULL); return    qe->documentIDsFromMetadata(attributeName, attributeValue); }
//extern "C" std::string QueryEnvironment_stemTerm(indri::api::QueryEnvironment* qe, const std::string &term)                 { assert(qe != NULL); return    qe->stemTerm(term); }
//extern "C" INT64 QueryEnvironment_termCountUnique(indri::api::QueryEnvironment* qe)                                         { assert(qe != NULL); return    qe->termCountUnique(); }
//extern "C" INT64 QueryEnvironment_termCount(indri::api::QueryEnvironment* qe)                                               { assert(qe != NULL); return    qe->termCount(); }
//extern "C" INT64 QueryEnvironment_termCount(indri::api::QueryEnvironment* qe, const std::string& term)                      { assert(qe != NULL); return    qe->termCount(term); }
//extern "C" INT64 QueryEnvironment_stemCount(indri::api::QueryEnvironment* qe, const std::string& term)                      { assert(qe != NULL); return    qe->stemCount(term); }
//extern "C" INT64 QueryEnvironment_termFieldCount(indri::api::QueryEnvironment* qe, const std::string& term,
//    const std::string& field)                                                                                               { assert(qe != NULL); return    qe->termFieldCount(term, field); }
//extern "C" INT64 QueryEnvironment_stemFieldCount(indri::api::QueryEnvironment* qe, const std::string& term,
//    const std::string& field)                                                                                               { assert(qe != NULL); return    qe->stemFieldCount(term, field); }
//extern "C" double QueryEnvironment_expressionCount(indri::api::QueryEnvironment* qe, const std::string& expression,
//    const std::string &queryType)                                                                                           { assert(qe != NULL); return    qe->expressionCount(expression, queryType); }
//extern "C" double QueryEnvironment_documentExpressionCount(indri::api::QueryEnvironment* qe, const std::string& expression,
//    const std::string &queryType)                                                                                           { assert(qe != NULL); return    qe->documentExpressionCount(expression, queryType); }
//extern "C" std::vector<ScoredExtentResult> QueryEnvironment_expressionList(indri::api::QueryEnvironment* qe,
//    const std::string& expression, const std::string& queryType)                                                            { assert(qe != NULL); return    qe->expressionList(expression, queryType); }
//extern "C" std::vector<std::string> QueryEnvironment_fieldList(indri::api::QueryEnvironment* qe)                            { assert(qe != NULL); return    qe->fieldList(); }
//extern "C" INT64 QueryEnvironment_documentCount(indri::api::QueryEnvironment* qe)                                           { assert(qe != NULL); return    qe->documentCount(); }
//extern "C" INT64 QueryEnvironment_documentCount(indri::api::QueryEnvironment* qe, const std::string& term)                  { assert(qe != NULL); return    qe->documentCount(term); }
//extern "C" INT64 QueryEnvironment_documentStemCount(indri::api::QueryEnvironment* qe, const std::string& stem)              { assert(qe != NULL); return    qe->documentStemCount(stem); }
//extern "C" int QueryEnvironment_documentLength(indri::api::QueryEnvironment* qe, lemur::api::DOCID_T documentID)            { assert(qe != NULL); return    qe->documentLength(documentID); }
//extern "C" std::vector<DocumentVector*> QueryEnvironment_documentVectors(indri::api::QueryEnvironment* qe,
//    const std::vector<lemur::api::DOCID_T>& documentIDs)                                                                    { assert(qe != NULL); return    qe->documentVectors(documentIDs); }
//extern "C" void QueryEnvironment_setMaxWildcardTerms(indri::api::QueryEnvironment* qe, int maxTerms)                        { assert(qe != NULL);           qe->setMaxWildcardTerms(maxTerms); }
//extern "C" const std::vector<indri::server::QueryServer*>& QueryEnvironment_getServers(indri::api::QueryEnvironment* qe)    { assert(qe != NULL); return    qe->getServers(); }
//extern "C" void QueryEnvironment_setFormulationParameters(indri::api::QueryEnvironment* qe, Parameters &p)                  { assert(qe != NULL);           qe->setFormulationParameters(p); }
//extern "C" std::string QueryEnvironment_reformulateQuery(indri::api::QueryEnvironment* qe, const std::string &query)        { assert(qe != NULL); return    qe->reformulateQuery(query); }
