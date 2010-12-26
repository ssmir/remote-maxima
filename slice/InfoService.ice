module IARnet {
    module InfoService {            
    
        ["java:type:java.util.ArrayList<String>"]
        sequence<string> StringSeq;
        
        ["java:type:java.util.HashMap<String, String>"]
        dictionary<string, string> StringMap;
        
        class ServiceInfo {
            string proxy;
            StringSeq types;
            string description;
            StringSeq tags;
            StringMap metadata;
            StringMap dynamicInfo;
            string publishedBy;
        };
        
        ["java:type:java.util.ArrayList<ServiceInfo>"]      
        sequence<ServiceInfo> ServiceInfoSeq;
        
        struct ServiceFilter {
            string proxy;
            string type;
            StringSeq tags;
            StringMap metadata;
            StringMap dynamicInfo;
        };                
        
        exception PublishedByAnotherClientException {
        };
        
        exception ServiceNotPublishedException {
        };
                
            
        interface InfoPublish {
            void publishService(ServiceInfo info) 
                throws PublishedByAnotherClientException;
            void unpublishService(string proxy) 
                throws ServiceNotPublishedException, PublishedByAnotherClientException;
            void updateDynamicInfo(string proxy, StringMap dynamicInfo) 
                throws ServiceNotPublishedException, PublishedByAnotherClientException;
        };
        
        interface InfoQuery {
            idempotent ServiceInfo findService(ServiceFilter filter); 
            idempotent ServiceInfoSeq findServices(ServiceFilter filter);
            idempotent ServiceInfo findServiceByType(string type);
            idempotent ServiceInfoSeq findServicesByType(string type);
            idempotent ServiceInfo findServiceByTag(string tag);
            idempotent ServiceInfoSeq findServicesByTag(string tag);
            idempotent ServiceInfo findServiceByMetadata(StringMap metadata);
            idempotent ServiceInfoSeq findServicesByMetadata(StringMap metadata);
        };
        
        interface InfoObserver {
            void init(ServiceInfoSeq infos);
            void servicePublished(ServiceInfo info);
            void serviceUnpublished(string proxy);
            void dynamicInfoUpdated(string proxy, StringMap dynamicInfo);
        };
        
        interface InfoMonitor {
            void addObserver(InfoObserver* observer, ServiceFilter filter);
            void removeObserver(InfoObserver* observer);
        };
        
    };
};