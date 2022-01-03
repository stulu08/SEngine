using System;
using System.Net;
using System.Net.Http;

namespace Hub
{
    using Version = String;

    public class Download
    {
        //https://stackoverflow.com/questions/20309158/c-sharp-checking-internet-connection
        public static bool CheckForInternetConnection()
        {
            try
            {
                using (var client = new WebClient())
                using (var stream = client.OpenRead("https://engine.stulu.de"))
                {
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }
        public static string readwebFile(string url)
        {
            HttpClient client = new HttpClient();
            var result = client.GetStringAsync(url);
            result.Wait();
            return result.Result;
        }
        public static Version getLatestVersion()
        {
            if (CheckForInternetConnection())
                return readwebFile("http://engine.stulu.de/download/latest.version").Replace("\n", "");
            else
                return "2.2.3";
        }
        public static Version[] getVersions()
        {
            if (CheckForInternetConnection())
                return readwebFile("http://engine.stulu.de/download/all.version").Split('\n');
            else
                return new Version[2]{ "2.2.2","2.2.3"};
        }
        public static void DownloadFile(string downloadURL, string fileName, DownloadProgressChangedEventHandler eventProgressChanged, System.ComponentModel.AsyncCompletedEventHandler eventFinish)
        {
            using (WebClient wc = new WebClient())
            {
                wc.DownloadProgressChanged += eventProgressChanged;
                wc.DownloadFileCompleted += eventFinish;
                wc.DownloadFileAsync(
                    new System.Uri(downloadURL),
                    fileName
                );
            }
        }
        public static void DownloadFile(string downloadURL, string fileName)
        {
            using (WebClient wc = new WebClient())
            {
                wc.DownloadFileAsync(
                    new System.Uri(downloadURL),
                    fileName
                );
            }
        }
        public static void DownloadEngine(string Version, string fileName, string platform, DownloadProgressChangedEventHandler eventProgressChanged, System.ComponentModel.AsyncCompletedEventHandler eventFinish)
        {
            string downloadURL = "https://engine.stulu.de/download/" + Version + "/" + platform.ToLower();
            DownloadFile(downloadURL, fileName, eventProgressChanged, eventFinish);
        }
    }
}
