/*************************************************************************
	> File Name: global.h
	> Author: 
	> Mail: 
	> Created Time: Wed 25 May 2016 08:39:42 PM PDT
 ************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H


#include "sugar_soapH.h"
#include "md5.h"
#include <pthread.h>
#include <time.h>

#define MODULE_SIZE 20
#define PTR_SIZE    64
#define PTR_SIZE_2  128

#define CRM_STACK_SIZE  (1024 * 1024)

#define SAFE_FREE(buf) if (buf) {\
        free(buf);\
        buf = NULL;}

typedef pthread_mutex_t Lock;        

enum module_name{
    CRM_MODULE_ACCOUNTS = 0,
    CRM_MODULE_CONTACTS,
    CRM_MODULE_LEADS,
    CRM_MODULE_CALLS
};

enum search_type {
    CRM_TYPE_NAME =0 ,
    CRM_TYPE_PHONE,
    CRM_TYPE_PHONEOFFICE,
    CRM_TYPE_PHONEWORK,
    CRM_TYPE_PHONEMOBILE,
    CRM_TYPE_PHONEHOME,
    CRM_TYPE_OTHER
};

typedef enum _DEQUEUE_RESULT {
    DEQUEUE_SUCCESS_AND_CONTINUE,
    DEQUEUE_SUCCESS_AND_EMPTY,
    DEQUEUE_FAILED_EMPTY
} DEQUEUE_RESULT;

typedef enum _ENQUEUE_RESULT {
    ENQUEUE_SUCCESS,
    ENQUEUE_FAILED_USER_DELETE
} ENQUEUE_RESULT;

/* server information for every action*/
typedef struct __sg_server_info {
    struct soap *soap;      /* soap */
    char *soap_endpoint;    /* server address */
}SGServerInfo;

/*return data of CRM login*/
typedef struct __sg_login_return {
    char *session_id;       /* login session ID*/
    char *user_id;          /* login user's ID*/
}SGLoginReturn;

/*return data of link modues*/
typedef struct __sg_link_return {
    int created;            /* link create success set 1, else 0*/
    int failed;             /* link set failed 1, success 0*/
    int deleted;            /* link deleted success set 1, else 0*/
}SGLinkReturn;

/* return data of searchAccounts etc*/
typedef struct __sg_search_result {
    char *id;               /*search result id*/
    char *module;           /*search result module(accounts,contacts,leads. etc)*/
}SGSearchResult;

/* call information for addNewCall*/
typedef struct __sg_call_item {
    char *name;             /*call name*/
    char *parent_type;      /* who own the call*/
    char *parent_id;        /*owner's id*/
    char *direction;        /*inbound or outbound*/
    char *description;      /* description for this call*/
    char *duration_minutes; /*call mintues*/
    char *duration_hours;   /*call hours*/
    char *date_start;       /*call start*/
    char *assigned_user_id; /*user id*/
}SGCallItem;

typedef struct __sg_calls_item {
    char                *call_id;   /* call id if null add else update*/
    SGCallItem            *call_item; /* one item */
    struct __sg_calls_item   *next;
}SGCallsItem;

typedef struct __sg_contact_item {
	char    *title;
	char    *first_name;
	char    *last_name;
	char    *phone_home;
	char    *phone_mobile;
	char    *phone_work;
	char    *phone_other;
	char    *phone_fax;
	char    *description;
} SGContactItem;

typedef struct __sg_account_item {
	char	*name;
	char	*phone_office;
	char	*phone_alternate;
	char	*phone_fax;
	char	*description;
}SGAccountItem;

typedef struct __sg_lead_item {
	char	*title;
	char	*first_name;
	char	*last_name;
	char    *phone_home;
	char    *phone_mobile;
	char    *phone_work;
	char    *phone_other;
	char    *phone_fax;
	char    *description;
}SGLeadItem;

typedef struct __sg_return_calls_id {
    char *return_call_id;
	struct __sg_return_calls_id *next;
}SGReturnCallsID;

typedef struct __sg_links_id {
	char	*id;
	struct __sg_links_id *next;
}SGLinksID;

typedef struct __sg_contacts_id {
	char	*id;
	struct __sg_contacts_id *next;
}SGContactsID;


typedef struct __sg_crm_info_task {
    SGLoginReturn     *login_return;
    SGServerInfo      *server_info;
}SGCRMInfoTask;

typedef struct __sg_crm_task {
    //LoginReturn     *login_return;
    //ServerInfo      *server_info;
    char            *search_string;
    char            *call_return_id;
    SGSearchResult  *search_result;
    SGCallItem      *call_item;
    SGLinkReturn    *link_return;
    //struct __sg_crm_task   *next;
} SGCRMTask;

typedef struct queue_node_t {
    SGCRMTask	        *value;
    struct queue_node_t *next;
} QueueNode;

typedef struct queue_t {
    QueueNode *head;
    QueueNode *end;
    Lock q_h_lock;
    Lock q_t_lock;
    int not_empty;
} Queue;

struct __sg_search_result * initSGSearchResult(struct __sg_search_result *search_result);
void cleanSGSearchResult(struct __sg_search_result *search_result);
struct __sg_call_item *initSGCallItem(struct __sg_call_item *call_item);
void cleanSGCallItem(struct __sg_call_item* call_item);
struct __sg_contact_item *initSGContactItem(struct __sg_contact_item *contact_item);
void cleanSGContactItem(struct __sg_contact_item *contact_item);
struct __sg_account_item *initSGAccountItem(struct __sg_account_item *account_item);
void cleanSGAccountItem(struct __sg_account_item *account_item);
struct __sg_lead_item *initSGLeadItem(struct __sg_lead_item *lead_item);
void cleanSGLeadItem(struct __sg_lead_item *lead_item);


struct __sg_server_info *initSGServerInfo(struct __sg_server_info *server_info);
void cleanSGServerInfo(struct __sg_server_info *server_info);
struct __sg_login_return *initSGLoginReturn(struct __sg_login_return *login_return);
void cleanSGLoginReturn(struct __sg_login_return *login_return);
struct __sg_link_return *initSGLinkReturn(struct __sg_link_return *link_return);
void cleanSGLinkReturn(struct __sg_link_return *link_return);
struct __sg_crm_info_task *initSGCRMInfoTask(struct __sg_crm_info_task *crm_info_task);
void cleanSGCRMInfoTask(struct __sg_crm_info_task *crm_info_task);
struct __sg_crm_task *initSGCRMTask(struct __sg_crm_task *crm_task);
void cleanSGCRMTask(struct __sg_crm_task *crm_task);
struct __sg_contacts_id *initSGContactsID(struct __sg_contacts_id *contacts_id);
void cleanSGContactsID(struct __sg_contacts_id *contacts_id);
struct __sg_links_id *initSGLinksID(struct __sg_links_id *links_id);
void cleanSGLinksID(struct __sg_links_id *links_id);
struct __sg_return_calls_id *initSGReturnCallsID(struct __sg_return_calls_id *return_calls_id);
void cleanSGReturnCallsID(struct __sg_return_calls_id *return_calls_id);

int sugarLogin(struct soap *soap, const char *soap_endpoint, char *login_name, char *login_pwd, struct ns1__loginResponse *_param_1);
int sugarLogout(struct soap *soap, const char *soap_endpoint, char *session, struct ns1__logoutResponse *_param_2);
int searchByModule(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *    
     session, char *search_USCOREstring, struct select_USCOREfields *modules, int offset, int max_USCOREresults, char *assigned_USCOREuser_USCOREid, struct 
     select_USCOREfields *select_USCOREfields, enum xsd__boolean unified_USCOREsearch_USCOREonly, enum xsd__boolean favorites, struct                       
     ns1__search_USCOREby_USCOREmoduleResponse *_param_17);
int getEntryList(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *      
     session, char *module_USCOREname, char *query, char *order_USCOREby, int offset, struct select_USCOREfields *select_USCOREfields, struct               
     link_USCOREnames_USCOREto_USCOREfields_USCOREarray *link_USCOREname_USCOREto_USCOREfields_USCOREarray, int max_USCOREresults, int deleted, enum        
     xsd__boolean favorites, struct ns1__get_USCOREentry_USCORElistResponse *_param_5);
int getEntries(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *
     module_USCOREname, struct select_USCOREfields *ids, struct select_USCOREfields *select_USCOREfields, struct                                            
     link_USCOREnames_USCOREto_USCOREfields_USCOREarray *link_USCOREname_USCOREto_USCOREfields_USCOREarray, enum xsd__boolean track_USCOREview, struct      
     ns1__get_USCOREentriesResponse *_param_4);
int getEntry(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *  
     module_USCOREname, char *id, struct select_USCOREfields *select_USCOREfields, struct link_USCOREnames_USCOREto_USCOREfields_USCOREarray *              
     link_USCOREname_USCOREto_USCOREfields_USCOREarray, enum xsd__boolean track_USCOREview, struct ns1__get_USCOREentryResponse *_param_3);
int setEntry(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session, char *  
     module_USCOREname, struct name_USCOREvalue_USCORElist *name_USCOREvalue_USCORElist, struct ns1__set_USCOREentryResponse *_param_9);
int setRelationship(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *session,  
     char *module_USCOREname, char *module_USCOREid, char *link_USCOREfield_USCOREname, struct select_USCOREfields *related_USCOREids, struct               
     name_USCOREvalue_USCORElist *name_USCOREvalue_USCORElist, int delete_, struct ns1__set_USCORErelationshipResponse *_param_6);
int getAvailableModules(struct soap *soap, const char *soap_endpoint, const char *soap_action,      
     char *session, char *filter, struct ns1__get_USCOREavailable_USCOREmodulesResponse *_param_18);
     
int searchModule(struct soap *soap, const char *soap_endpoint, char *session, enum module_name module_name, char *search_string,
                enum search_type search_type, SGSearchResult *search_result);

int setCall(struct soap *soap, const char *soap_endpoint, char *session, char *call_id, SGCallItem *call_item, char **return_id);
int setCallRelation(struct soap *soap, const char *server, char *session_id, char *call_id, char *link_name, char *link_id, SGLinkReturn *link_return);
int setCalls(struct soap *soap, const char *soap_endpoint, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id);

int setContactRelation(struct soap *soap, const char *soap_endpoint, char *session, char *call_id, char *link_name, char *link_id, SGLinkReturn *link_return);
int setContactRelations(struct soap * soap,const char * soap_endpoint,char * session, SGContactsID *contacts_id, 
								char *link_name, SGLinksID *links_id, int num, SGLinkReturn *link_return);

int setContact(struct soap *soap, const char *soap_endpoint, char *session, char *contact_id, SGContactItem *contact_item, char **return_id);
int setAccount(struct soap *soap, const char *soap_endpoint, char *session, char *account_id, SGAccountItem *account_item, char **return_id);
int setLead(struct soap *soap, const char *soap_endpoint, char *session, char *lead_id, SGLeadItem *lead_item, char **return_id);


int SGCRMLogin(SGServerInfo *server_info, char *username, char *password, SGLoginReturn *login_return);
int SGCRMLogout(SGServerInfo *server_info, char *session);
int addCall(SGServerInfo *server_info, char *session, SGCallItem *call_item, char **return_call_id);
int updateCall(SGServerInfo *server_info, char *session, char *call_id, SGCallItem *call_item, char **return_call_id);
int updateCallBatch(SGServerInfo *server_info, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id);
int addCallBatch(SGServerInfo *server_info, char *session, SGCallsItem *calls_item, SGReturnCallsID *return_calls_id);
int addContact(SGServerInfo *server_info, char *session, SGContactItem *contact_item, char **return_contact_id);
int addAccount(SGServerInfo *server_info, char *session, SGAccountItem *account_item, char **return_account_id);

int searchAccounts(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result);
int searchContacts(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result);
int searchLeads(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result);
int searchCalls(SGServerInfo *server_info, char *session, char *search_string, SGSearchResult *search_result);
int callLinkUser(SGServerInfo *server_info, char *session, char *call_id, char *user_id, SGLinkReturn *link_return);
int callLinkContact(SGServerInfo *server_info, char *session, char *call_id, char *contact_id, SGLinkReturn *link_return);
int callLinkLead(SGServerInfo *server_info, char *session, char *call_id, char *lead_id, SGLinkReturn *link_return);
int contactLinkCall(SGServerInfo *server_info, char * session,char *contact_id, char *call_id, SGLinkReturn * link_return);
int contactsLinkCalls(SGServerInfo *server_info, char *session, SGContactsID *contacts_id, SGLinksID *links_id, int num, SGLinkReturn *link_return);


void queueInitialize(Queue *queue);
void cleanQueue(Queue *queue);
ENQUEUE_RESULT enqueueTask(Queue *q, SGCRMTask *task);
DEQUEUE_RESULT dequeueTask(Queue *q, SGCRMTask **pvalue);

#endif
