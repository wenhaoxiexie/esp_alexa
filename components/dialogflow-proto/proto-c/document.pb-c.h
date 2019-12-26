/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: google/cloud/dialogflow/v2beta1/document.proto */

#ifndef PROTOBUF_C_google_2fcloud_2fdialogflow_2fv2beta1_2fdocument_2eproto__INCLUDED
#define PROTOBUF_C_google_2fcloud_2fdialogflow_2fv2beta1_2fdocument_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003000 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif

#include "annotations.pb-c.h"
#include "operations.pb-c.h"
#include "empty.pb-c.h"
#include "field_mask.pb-c.h"

typedef struct _Google__Cloud__Dialogflow__V2beta1__Document Google__Cloud__Dialogflow__V2beta1__Document;
typedef struct _Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest;
typedef struct _Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse;
typedef struct _Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest;
typedef struct _Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest;
typedef struct _Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest;
typedef struct _Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata;


/* --- enums --- */

/*
 * The knowledge type of document content.
 */
typedef enum _Google__Cloud__Dialogflow__V2beta1__Document__KnowledgeType {
  /*
   * The type is unspecified or arbitrary.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__KNOWLEDGE_TYPE__KNOWLEDGE_TYPE_UNSPECIFIED = 0,
  /*
   * The document content contains question and answer pairs as either HTML or
   * CSV. Typical FAQ HTML formats are parsed accurately, but unusual formats
   * may fail to be parsed.
   * CSV must have questions in the first column and answers in the second,
   * with no header. Because of this explicit format, they are always parsed
   * accurately.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__KNOWLEDGE_TYPE__FAQ = 1,
  /*
   * Documents for which unstructured text is extracted and used for
   * question answering.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__KNOWLEDGE_TYPE__EXTRACTIVE_QA = 2
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__KNOWLEDGE_TYPE)
} Google__Cloud__Dialogflow__V2beta1__Document__KnowledgeType;
/*
 * States of the operation.
 */
typedef enum _Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata__State {
  /*
   * State unspecified.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE__STATE_UNSPECIFIED = 0,
  /*
   * The operation has been created.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE__PENDING = 1,
  /*
   * The operation is currently running.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE__RUNNING = 2,
  /*
   * The operation is done, either cancelled or completed.
   */
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE__DONE = 3
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE)
} Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata__State;

/* --- messages --- */

typedef enum {
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__SOURCE__NOT_SET = 0,
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__SOURCE_CONTENT_URI = 5,
  GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__SOURCE_CONTENT = 6
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__SOURCE)
} Google__Cloud__Dialogflow__V2beta1__Document__SourceCase;

/*
 * A document resource.
 */
struct  _Google__Cloud__Dialogflow__V2beta1__Document
{
  ProtobufCMessage base;
  /*
   * The document resource name.
   * The name must be empty when creating a document.
   * Format: `projects/<Project ID>/knowledgeBases/<Knowledge Base
   * ID>/documents/<Document ID>`.
   */
  char *name;
  /*
   * Required. The display name of the document. The name must be 1024 bytes or
   * less; otherwise, the creation request fails.
   */
  char *display_name;
  /*
   * Required. The MIME type of this document.
   */
  char *mime_type;
  /*
   * Required. The knowledge type of document content.
   */
  size_t n_knowledge_types;
  Google__Cloud__Dialogflow__V2beta1__Document__KnowledgeType *knowledge_types;
  Google__Cloud__Dialogflow__V2beta1__Document__SourceCase source_case;
  union {
    /*
     * The URI where the file content is located.
     * For documents stored in Google Cloud Storage, these URIs must have
     * the form `gs://<bucket-name>/<object-name>`.
     * NOTE: External URLs must correspond to public webpages, i.e., they must
     * be indexed by Google Search. In particular, URLs for showing documents in
     * Google Cloud Storage (i.e. the URL in your browser) are not supported.
     * Instead use the `gs://` format URI described above.
     */
    char *content_uri;
    /*
     * The raw content of the document. This field is only permitted for
     * EXTRACTIVE_QA and FAQ knowledge types.
     */
    char *content;
  };
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__document__descriptor) \
    , (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, (char *)protobuf_c_empty_string, 0,NULL, GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENT__SOURCE__NOT_SET, {0} }


/*
 * Request message for [Documents.ListDocuments][google.cloud.dialogflow.v2beta1.Documents.ListDocuments].
 */
struct  _Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest
{
  ProtobufCMessage base;
  /*
   * Required. The knowledge base to list all documents for.
   * Format: `projects/<Project ID>/knowledgeBases/<Knowledge Base ID>`.
   */
  char *parent;
  /*
   * Optional. The maximum number of items to return in a single page. By
   * default 10 and at most 100.
   */
  int32_t page_size;
  /*
   * Optional. The next_page_token value returned from a previous list request.
   */
  char *page_token;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__LIST_DOCUMENTS_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__list_documents_request__descriptor) \
    , (char *)protobuf_c_empty_string, 0, (char *)protobuf_c_empty_string }


/*
 * Response message for [Documents.ListDocuments][google.cloud.dialogflow.v2beta1.Documents.ListDocuments].
 */
struct  _Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse
{
  ProtobufCMessage base;
  /*
   * The list of documents.
   */
  size_t n_documents;
  Google__Cloud__Dialogflow__V2beta1__Document **documents;
  /*
   * Token to retrieve the next page of results, or empty if there are no
   * more results in the list.
   */
  char *next_page_token;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__LIST_DOCUMENTS_RESPONSE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__list_documents_response__descriptor) \
    , 0,NULL, (char *)protobuf_c_empty_string }


/*
 * Request message for [Documents.GetDocument][google.cloud.dialogflow.v2beta1.Documents.GetDocument].
 */
struct  _Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest
{
  ProtobufCMessage base;
  /*
   * Required. The name of the document to retrieve.
   * Format `projects/<Project ID>/knowledgeBases/<Knowledge Base
   * ID>/documents/<Document ID>`.
   */
  char *name;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__GET_DOCUMENT_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__get_document_request__descriptor) \
    , (char *)protobuf_c_empty_string }


/*
 * Request message for [Documents.CreateDocument][google.cloud.dialogflow.v2beta1.Documents.CreateDocument].
 */
struct  _Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest
{
  ProtobufCMessage base;
  /*
   * Required. The knoweldge base to create a document for.
   * Format: `projects/<Project ID>/knowledgeBases/<Knowledge Base ID>`.
   */
  char *parent;
  /*
   * Required. The document to create.
   */
  Google__Cloud__Dialogflow__V2beta1__Document *document;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__CREATE_DOCUMENT_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__create_document_request__descriptor) \
    , (char *)protobuf_c_empty_string, NULL }


/*
 * Request message for [Documents.DeleteDocument][google.cloud.dialogflow.v2beta1.Documents.DeleteDocument].
 */
struct  _Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest
{
  ProtobufCMessage base;
  /*
   * The name of the document to delete.
   * Format: `projects/<Project ID>/knowledgeBases/<Knowledge Base
   * ID>/documents/<Document ID>`.
   */
  char *name;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DELETE_DOCUMENT_REQUEST__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__delete_document_request__descriptor) \
    , (char *)protobuf_c_empty_string }


/*
 * Metadata in google::longrunning::Operation for Knowledge operations.
 */
struct  _Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata
{
  ProtobufCMessage base;
  /*
   * Required. The current state of this operation.
   */
  Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata__State state;
};
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__descriptor) \
    , GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__KNOWLEDGE_OPERATION_METADATA__STATE__STATE_UNSPECIFIED }


/* Google__Cloud__Dialogflow__V2beta1__Document methods */
void   google__cloud__dialogflow__v2beta1__document__init
                     (Google__Cloud__Dialogflow__V2beta1__Document         *message);
size_t google__cloud__dialogflow__v2beta1__document__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__Document   *message);
size_t google__cloud__dialogflow__v2beta1__document__pack
                     (const Google__Cloud__Dialogflow__V2beta1__Document   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__document__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__Document   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__Document *
       google__cloud__dialogflow__v2beta1__document__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__document__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__Document *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest methods */
void   google__cloud__dialogflow__v2beta1__list_documents_request__init
                     (Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest         *message);
size_t google__cloud__dialogflow__v2beta1__list_documents_request__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest   *message);
size_t google__cloud__dialogflow__v2beta1__list_documents_request__pack
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__list_documents_request__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest *
       google__cloud__dialogflow__v2beta1__list_documents_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__list_documents_request__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse methods */
void   google__cloud__dialogflow__v2beta1__list_documents_response__init
                     (Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse         *message);
size_t google__cloud__dialogflow__v2beta1__list_documents_response__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse   *message);
size_t google__cloud__dialogflow__v2beta1__list_documents_response__pack
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__list_documents_response__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse *
       google__cloud__dialogflow__v2beta1__list_documents_response__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__list_documents_response__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest methods */
void   google__cloud__dialogflow__v2beta1__get_document_request__init
                     (Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest         *message);
size_t google__cloud__dialogflow__v2beta1__get_document_request__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest   *message);
size_t google__cloud__dialogflow__v2beta1__get_document_request__pack
                     (const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__get_document_request__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest *
       google__cloud__dialogflow__v2beta1__get_document_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__get_document_request__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest methods */
void   google__cloud__dialogflow__v2beta1__create_document_request__init
                     (Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest         *message);
size_t google__cloud__dialogflow__v2beta1__create_document_request__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest   *message);
size_t google__cloud__dialogflow__v2beta1__create_document_request__pack
                     (const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__create_document_request__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest *
       google__cloud__dialogflow__v2beta1__create_document_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__create_document_request__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest methods */
void   google__cloud__dialogflow__v2beta1__delete_document_request__init
                     (Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest         *message);
size_t google__cloud__dialogflow__v2beta1__delete_document_request__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest   *message);
size_t google__cloud__dialogflow__v2beta1__delete_document_request__pack
                     (const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__delete_document_request__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest *
       google__cloud__dialogflow__v2beta1__delete_document_request__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__delete_document_request__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest *message,
                      ProtobufCAllocator *allocator);
/* Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata methods */
void   google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__init
                     (Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata         *message);
size_t google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__get_packed_size
                     (const Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata   *message);
size_t google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__pack
                     (const Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata   *message,
                      uint8_t             *out);
size_t google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__pack_to_buffer
                     (const Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata   *message,
                      ProtobufCBuffer     *buffer);
Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata *
       google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__free_unpacked
                     (Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Google__Cloud__Dialogflow__V2beta1__Document_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__Document *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest *message,
                  void *closure_data);
typedef void (*Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata_Closure)
                 (const Google__Cloud__Dialogflow__V2beta1__KnowledgeOperationMetadata *message,
                  void *closure_data);

/* --- services --- */

typedef struct _Google__Cloud__Dialogflow__V2beta1__Documents_Service Google__Cloud__Dialogflow__V2beta1__Documents_Service;
struct _Google__Cloud__Dialogflow__V2beta1__Documents_Service
{
  ProtobufCService base;
  void (*list_documents)(Google__Cloud__Dialogflow__V2beta1__Documents_Service *service,
                         const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest *input,
                         Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse_Closure closure,
                         void *closure_data);
  void (*get_document)(Google__Cloud__Dialogflow__V2beta1__Documents_Service *service,
                       const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest *input,
                       Google__Cloud__Dialogflow__V2beta1__Document_Closure closure,
                       void *closure_data);
  void (*create_document)(Google__Cloud__Dialogflow__V2beta1__Documents_Service *service,
                          const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest *input,
                          Google__Longrunning__Operation_Closure closure,
                          void *closure_data);
  void (*delete_document)(Google__Cloud__Dialogflow__V2beta1__Documents_Service *service,
                          const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest *input,
                          Google__Longrunning__Operation_Closure closure,
                          void *closure_data);
};
typedef void (*Google__Cloud__Dialogflow__V2beta1__Documents_ServiceDestroy)(Google__Cloud__Dialogflow__V2beta1__Documents_Service *);
void google__cloud__dialogflow__v2beta1__documents__init (Google__Cloud__Dialogflow__V2beta1__Documents_Service *service,
                                                          Google__Cloud__Dialogflow__V2beta1__Documents_ServiceDestroy destroy);
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENTS__BASE_INIT \
    { &google__cloud__dialogflow__v2beta1__documents__descriptor, protobuf_c_service_invoke_internal, NULL }
#define GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENTS__INIT(function_prefix__) \
    { GOOGLE__CLOUD__DIALOGFLOW__V2BETA1__DOCUMENTS__BASE_INIT,\
      function_prefix__ ## list_documents,\
      function_prefix__ ## get_document,\
      function_prefix__ ## create_document,\
      function_prefix__ ## delete_document  }
void google__cloud__dialogflow__v2beta1__documents__list_documents(ProtobufCService *service,
                                                                   const Google__Cloud__Dialogflow__V2beta1__ListDocumentsRequest *input,
                                                                   Google__Cloud__Dialogflow__V2beta1__ListDocumentsResponse_Closure closure,
                                                                   void *closure_data);
void google__cloud__dialogflow__v2beta1__documents__get_document(ProtobufCService *service,
                                                                 const Google__Cloud__Dialogflow__V2beta1__GetDocumentRequest *input,
                                                                 Google__Cloud__Dialogflow__V2beta1__Document_Closure closure,
                                                                 void *closure_data);
void google__cloud__dialogflow__v2beta1__documents__create_document(ProtobufCService *service,
                                                                    const Google__Cloud__Dialogflow__V2beta1__CreateDocumentRequest *input,
                                                                    Google__Longrunning__Operation_Closure closure,
                                                                    void *closure_data);
void google__cloud__dialogflow__v2beta1__documents__delete_document(ProtobufCService *service,
                                                                    const Google__Cloud__Dialogflow__V2beta1__DeleteDocumentRequest *input,
                                                                    Google__Longrunning__Operation_Closure closure,
                                                                    void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__document__descriptor;
extern const ProtobufCEnumDescriptor    google__cloud__dialogflow__v2beta1__document__knowledge_type__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__list_documents_request__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__list_documents_response__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__get_document_request__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__create_document_request__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__delete_document_request__descriptor;
extern const ProtobufCMessageDescriptor google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__descriptor;
extern const ProtobufCEnumDescriptor    google__cloud__dialogflow__v2beta1__knowledge_operation_metadata__state__descriptor;
extern const ProtobufCServiceDescriptor google__cloud__dialogflow__v2beta1__documents__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_google_2fcloud_2fdialogflow_2fv2beta1_2fdocument_2eproto__INCLUDED */
