#ifndef DUriH
#define DUriH

/**
  // See: Apendix B from http://www.ietf.org/rfc/rfc2396.txt for regular expression
  Decode:
The following line is the regular expression for breaking-down a URI
   reference into its components.

      ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
       12            3  4          5       6  7        8 9

   The numbers in the second line above are only to assist readability;
   they indicate the reference points for each subexpression (i.e., each
   paired parenthesis).  We refer to the value matched for subexpression
   <n> as $<n>.  For example, matching the above expression to

      http://www.ics.uci.edu/pub/ietf/uri/#Related

   results in the following subexpression matches:

      $1 = http:
      $2 = http
      $3 = //www.ics.uci.edu
      $4 = www.ics.uci.edu
      $5 = /pub/ietf/uri/
      $6 = <undefined>
      $7 = <undefined>
      $8 = #Related
      $9 = Related

   where <undefined> indicates that the component is not present, as is
   the case for the query component in the above example.  Therefore, we
   can determine the value of the four components and fragment as

      scheme    = $2
      authority = $4
      path      = $5
      query     = $7
      fragment  = $9

      encode:
The resulting URI components, including any inherited from the
      base URI, are recombined to give the absolute form of the URI
      reference.  Using pseudocode, this would be

         result = ""

         if scheme is defined then
             append scheme to result
             append ":" to result

         if authority is defined then
             append "//" to result
             append authority to result

         append path to result

         if query is defined then
             append "?" to result
             append query to result

         if fragment is defined then
             append "#" to result
             append fragment to result

         return result

      Note that we must be careful to preserve the distinction between a
      component that is undefined, meaning that its separator was not
      present in the reference, and a component that is empty, meaning
      that the separator was present and was immediately followed by the
      next component separator or the end of the reference.
*/

#include <string>

namespace DTools {
namespace DNetwork {

class DUri {
    private:
        const std::string URI_VALIDATE_REGEX=R"([a-z][a-z0-9+.-]*:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()!@:%_\+.~#?&\/\/=]*))";
        const std::string URL_VALIDATE_REGEX=R"(https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()!@:%_\+.~#?&\/\/=]*))";
        const std::string RFC2396_SPLIT_REGEX=R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)";
        const std::string AUTHORITY_SPLIT_REGEX=R"((?:([^@:]*)(?::([^@]*))?@)?(\[[^\]]*\]|[^\[:]*)(?::(\d*))?)";
        /*
              "(?:([^@:]*)(?::([^@]*))?@)?" // username, password
              "(\\[[^\\]]*\\]|[^\\[:]*)" // host (IP-literal (e.g. '['+IPv6+']',
                                         // dotted-IPv4, or named host)
              "(?::(\\d*))?"); // port
        */

        enum DUriMatches {
            MATCH_URI_SCHEME    = 2,
            MATCH_URI_AUTHORITY = 4,
            MATCH_URI_PATH      = 5,
            MATCH_URI_QUERY     = 7,
            MATCH_URI_FRAGMENT  = 9
        };
        enum DAuthMatch {
            MATCH_AUTH_USER  = 1,
            MATCH_AUTH_PWD   = 2,
            MATCH_AUTH_HOST  = 3,
            MATCH_AUTH_PORT  = 4
        };

        bool Valid;
        std::string LastStrStatus;

        bool Parse(void);
        void Log(std::string LogMsg);
        void Error(std::string ErrorMsg);

    public:

        // Uri parts
        std::string UriString;
        std::string Scheme;
        std::string Authority;
        std::string Path;
        std::string Query;
        std::string Fragment;
        // Authority parts
        std::string User;
        std::string Pwd;
        std::string Host;
        std::string Port;


        DUri (std::string Uri = std::string());
        bool Set(std::string Uri);
        void Clear(void);
        bool IsValid(void);
        std::string PrintUriParts(void);
        static std::string EncodeUserInfo(const std::string& UserInfoStr);
        static std::string EncodeHost(const std::string& HostStr);
        static std::string EncodePath(const std::string& PathStr);
        static std::string EncodeQuery(const std::string& QueryStr);
        static std::string EncodeFragment(const std::string& FragmentStr);
        static std::string Encode(std::string Uri, std::string Ignore = std::string());
        static std::string decode(const std::string& EncodedUri);
        std::string GetLastStatus(void);

};

}
}

#endif
