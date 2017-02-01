# Luandri
A Clean Lua Interface to the [Indri Search Engine](http://www.lemurproject.org/indri) for [Torch](http://torch.ch).

Citation
--------
Coming soon.

Sample code
--------
How to query an existing index using runQuery?

    local luandri = paths.dofile('luandri.lua')
    
    local query_environment = QueryEnvironment()
    query_environment:addIndex("path_to_index_file")
    local request = { query = "neural networks", resultsRequested = 5 }
    local results = query_environment:runQuery(request).results
    
    for k, v in pairs(results) do
        print(v.docid .. '\t' .. v.documentName .. '\t' .. v.snippet)
    end

License
-------
Luandri is licensed under the [MIT license](LICENSE). Please check the [Lemur project](http://www.lemurproject.org/license.html) for licensing information on Indri. Finally, if you modify or extend Luandri in any way then please link back to this repository.