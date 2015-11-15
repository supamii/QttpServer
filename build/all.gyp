{
    'targets' : [
        #server
        {
            'target_name' : 'webserver',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native'
            ],
            'sources' : [
                '../lib/http/samples/webserver.cpp'
            ]
        },
        #client
        {
            'target_name' : 'webclient',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native'
            ],
            'sources' : [
                '../lib/http/samples/webclient.cpp'
            ]
        },
        #test
        {
            'target_name' : 'test',
            'type' : 'executable',
            'dependencies': [
                './native.gyp:node_native',
                './gmock.gyp:gmock_main'
            ],
            'sources' : [
                '../lib/http/test/basic_test.cc'
            ]
        }
    ]
}
