#ifdef IN_CELLML_MODULE
#define MODULE_CONTAINS_RDFAPISPEC
#endif
#ifdef MODULE_CONTAINS_RDFAPISPEC
#define RDF_PUBLIC_PRE CDA_EXPORT_PRE
#define RDF_PUBLIC_POST CDA_EXPORT_POST
#else
#define RDF_PUBLIC_PRE CDA_IMPORT_PRE
#define RDF_PUBLIC_POST CDA_IMPORT_POST
#endif

RDF_PUBLIC_PRE already_AddRefd<iface::rdf_api::Bootstrap> CreateRDFBootstrap()
  RDF_PUBLIC_POST;
