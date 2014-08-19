{
    'targets' : [
        #test
        {
            'target_name' : 'gtest',
            'type' : 'static_library',
            'include_dirs' : [
                '../deps/gtest/include',
                '../deps/gtest'
            ],
            'sources' : [
                '../deps/gtest/src/gtest-all.cc',
                '../deps/gtest/src/gtest-death-test.cc',
                '../deps/gtest/src/gtest-filepath.cc',
                '../deps/gtest/src/gtest-port.cc',
                '../deps/gtest/src/gtest-printers.cc',
                '../deps/gtest/src/gtest-test-part.cc',
                '../deps/gtest/src/gtest-typed-test.cc',
                '../deps/gtest/src/gtest.cc',
                '../deps/gtest/src/gtest_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/gtest/include'
                ]
            }
        }
    ]
}
