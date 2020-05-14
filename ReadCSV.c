  bool read_csv( XlFile* xlf, const tchar_t* filename )
    {
        xlf->New();
        xlf->AddSheet( _T( "sheet01" ) );
        FILE* csvf = _tfopen( filename, _T( "rb" ) );
        if ( csvf != NULL )
        {
            size64_t flen = _filelength( fileno( csvf ) );
            char* buff = new char[ flen + 1 ];
            uint_t rc = 1;
            while ( fgets( buff, flen, csvf ) != NULL )  // 讀一行
            {
                uint_t cc = 1;
                _tstring line_string;
#ifdef _UNICODE
                line_string = pfstd::Utf8ToUnicode( buff );

#else
                line_string = buff;
#endif

                if ( buff[ 0 ] == -17 )
                {
                    line_string.erase( 0, 1 ); //bom
                }

                if ( buff[ strlen( buff ) - 1 ] == '\n' )
                {
                    line_string.erase( line_string.end() - 1, line_string.end() ); // /r/
                }

                if ( buff[ strlen( buff ) - 2 ] == '\r' )
                {
                    line_string.erase( line_string.end() - 1, line_string.end() ); // /r/
                }

                line_string.push_back( _T( ',' ) );

                std::vector<tchar_t> cell_data;
                size64_t llen = line_string.size();
                bool st = false;
                for ( uint_t li = 0; li < llen; li++ )
                {
                    if ( line_string[ li ] == _T( '"' ) )
                    {
                        li++;
                        // 可能是一個字串的結束
                        if ( st )
                        {
                            if ( line_string[ li ] == _T( ',' ) )
                            {
                                cell_data.push_back( _T('\0') );
                                ( cell_data.empty() ) ? xlf->SelectCell( cc, rc )->SetCell( _T( "" ) ) : xlf->SelectCell( cc, rc )->SetCell( &cell_data[ 0 ] );
                                cell_data.clear();
                                cc++;
                                st = false; //回復
                                continue;
                            }
                        }
                        else
                        {
                            st = true;
                        }

                        cell_data.push_back( line_string[ li ] );
                        continue;

                    }
                    else if ( line_string[ li ] == _T( ',' ) )
                    {
                        if ( li > 0 )
                        {
                            // 可能是一個字串的結束
                            // 在格子內的,
                            if ( st )
                            {
                                cell_data.push_back( line_string[ li ] );
                                continue;
                            }
                        }

                        cell_data.push_back( _T( '\0' ) );
                        ( cell_data.empty() ) ? xlf->SelectCell( cc, rc )->SetCell( _T( "" ) ) : xlf->SelectCell( cc, rc )->SetCell( &cell_data[ 0 ] );
                        cell_data.clear();
                        cc++;
                        continue;
                    }
                    else
                    {
                        cell_data.push_back( line_string[ li ] );
                    }
                }

                rc++;
            }
            SAFE_DELETE_ARRAY( buff );
            fclose( csvf );
        }

        {
            TCHAR drive[ _MAX_DRIVE ];
            TCHAR dir[ _MAX_DIR ];
            TCHAR fname[ _MAX_FNAME ];
            TCHAR ext[ _MAX_EXT ];
            _tsplitfpath( filename, drive, dir, fname, ext );

            TCHAR savefname[ _MAX_PATH ];
            _tmakefpath( savefname, drive, dir, fname, _T( ".xlsx" ) );
            xlf->Save( savefname );
        }
        return true;
    }