using System;
using System.IO.Compression;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Navigation;
using System.IO;
using Microsoft.WindowsAPICodePack.Dialogs;
using System.Net;

namespace Launcher
{

    using Platform = String;
    using Version = String;
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            if (!Directory.Exists(dataFolderPath))
                Directory.CreateDirectory(dataFolderPath);
            
            
            InitializeComponent();
            setSelectedMenu(Menu.Home);
            loadProjects();
            loadEngines();
            loadVersions();
            canInstall();
            engineInstallDirectory.Text = (Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles) + "/Stulu/Engine").Replace("\\", "/");
            this.Closed += destruct;
            saveFiles();
            if (!Download.CheckForInternetConnection())
            {
                MessageBox.Show("Can't connect to server http://engine.stulu.de", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
        }
        void destruct (object sender, EventArgs e)
        {
            saveFiles();
        }
        readonly private string dataFolderPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + "/stulu/Stulu Hub";
        #region Engines
        Version latestVersion = Download.getLatestVersion();
        Version[] versions = Download.getVersions();
        struct Engine
        {
            public string path;
            public Version version;
            public Platform platform;
            public Engine(string path, string version, string platform)
            {
                this.path = path;
                this.version = version;
                this.platform = platform;
            }
        }
        List<Engine> engines = new List<Engine>();
        List<Platform> platforms = new List<Platform>() { "Windows"};
        List<Platform> platformExtension = new List<Platform>() { ".zip"};

        string downloadFileName = "";
        void installEngine()
        {
            if (!Download.CheckForInternetConnection())
            {
                MessageBox.Show("Can't connect to server http://engine.stulu.de", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            if (!canInstall())
            {
                MessageBox.Show("Select a verison, path, platform", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            EngineDownloadInfo_ProgressLabel.Content = "0% 0.00mb / 0.00mb";
            EngineDownloadInfo_InfoLabel.Content = "Downloading";
            EngineDownloadInfo_ProgressBar.Value = 0.0d;
            EngineDownloadInfo.Visibility = Visibility.Visible;
            EngineDownloadInfo.IsEnabled = true;
            downloadFileName = Path.GetTempFileName();
            Download.DownloadEngine(versions[engineInstallVersionSelectList.SelectedIndex], downloadFileName, 
                platforms[engineInstallPlatformSelectList.SelectedIndex] + platformExtension[engineInstallPlatformSelectList.SelectedIndex]
                , asyncDownloadCallBackProgressUpdate, asyncDownloadCallBackFinish);
            
        }
        void asyncDownloadCallBackProgressUpdate(object sender, DownloadProgressChangedEventArgs e)
        {
            EngineDownloadInfo_InfoLabel.Content = "Downloading";
            EngineDownloadInfo_ProgressBar.Value = Math.Min(e.ProgressPercentage, 99);
            EngineDownloadInfo_ProgressLabel.Content = String.Format("{0}% {1:0.###}mb / {2:0.###}mb", Math.Min(e.ProgressPercentage, 99), e.BytesReceived / 1024f / 1024f, e.TotalBytesToReceive / 1024f / 1024f);
        }
        void asyncDownloadCallBackFinish(object sender, System.ComponentModel.AsyncCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                MessageBox.Show(e.Error.Message, "Error Downloading File", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            FileInfo info = new FileInfo(downloadFileName);
            EngineDownloadInfo_ProgressBar.Value = 99.0d;
            EngineDownloadInfo_ProgressLabel.Content = String.Format("99% {0:0.###}mb / {0:0.###}mb", info.Length / 1024f / 1024f);
            EngineDownloadInfo_InfoLabel.Content = "Extracting...";
            if (!Directory.Exists(engineInstallDirectory.Text))
                Directory.CreateDirectory(engineInstallDirectory.Text);
            ZipFile.ExtractToDirectory(downloadFileName, engineInstallDirectory.Text);
            File.Delete(downloadFileName);
            EngineDownloadInfo_ProgressLabel.Content = String.Format("100% {0:0.###}mb / {0:0.###}mb", info.Length / 1024f / 1024f);
            EngineDownloadInfo_InfoLabel.Content = "Finished";

            //add engine
            string ss = Directory.GetDirectories(engineInstallDirectory.Text)[0].Replace("\\", "/");
            Engine en = new Engine(ss, versions[engineInstallVersionSelectList.SelectedIndex], platforms[engineInstallPlatformSelectList.SelectedIndex]);
            ListBoxItem item = new ListBoxItem();
            item.Content = en.path + "\n" + en.version + "\n" + en.platform;
            item.FontSize = 18;
            EnginesListBox.Items.Add(item);
            engines.Add(en);
            selectedEngine = engines.IndexOf(en);
            saveFiles();
        }
        void loadEngines()
        {
            if (!File.Exists(dataFolderPath + "/engines.st"))
                    File.Create(dataFolderPath + "/engines.st").Close();
            string[] Rawengines = File.ReadAllLines(dataFolderPath + "/engines.st");
            foreach (string s in Rawengines)
            {
                if (s.StartsWith("default|"))
                {
                    continue;
                }
                string ss = s.Replace("\\", "/");
                if (!ss.Contains("|"))
                    continue;
                Engine e = new Engine(ss.Split('|')[0], ss.Split('|')[1], ss.Split('|')[2]);
                if (!isEngineValid(e))
                    continue;
                ListBoxItem item = new ListBoxItem();
                item.Content = e.path + "\n" + e.version + "\n" + e.platform;
                item.FontSize = 18;
                EnginesListBox.Items.Add(item);
                engines.Add(e);
            }
            //to prevent null error
            foreach (string s in Rawengines)
            {
                if (s.StartsWith("default|"))
                {
                    EnginesListBox.SelectedIndex = Convert.ToInt32(s.Split('|')[1]);
                }
            }
        }
        void loadVersions()
        {
            ListBoxItem item;
            {//veriosns
                for (int i = 0; i < versions.Length; i++)
                {
                    item = new ListBoxItem();
                    item.Content = versions[i] + (versions[i] == latestVersion ? "(latest)" : "");
                    item.FontSize = 18;
                    engineInstallVersionSelectList.Items.Add(item);
                }
            }
            {//platforms
                for (int i = 0; i < platforms.Count; i++)
                {
                    item = new ListBoxItem();
                    item.Content = platforms[i];
                    item.FontSize = 18;
                    engineInstallPlatformSelectList.Items.Add(item);
                }

            }
        }
        bool canInstall()
        {
            bool value = false;
            if (EnginePageInstallButton == null || engineInstallDirectory == null || engineInstallVersionSelectList == null || engineInstallPlatformSelectList == null)
            {
                return value;
            }
            value = engineInstallVersionSelectList.SelectedIndex > -1 && engineInstallPlatformSelectList.SelectedIndex > -1;
            if (value)
            {
                EnginePageInstallButton.Content = "Install Engine Version " + versions[engineInstallVersionSelectList.SelectedIndex] + " for " + platforms[engineInstallPlatformSelectList.SelectedIndex];
            }
            return value;
        }
        #endregion
        #region Projects
        struct Project
        {
            public string path;
            public string name;
            public Project(string path, string name)
            {
                this.path = path;
                this.name = name;
            }
        }
        List<Project> projects = new List<Project>();
        private void loadProjects()
        {
            if (!File.Exists(dataFolderPath + "/projects.st"))
                File.Create(dataFolderPath + "/projects.st").Close();
            string[] Rawprojects = File.ReadAllLines(dataFolderPath + "/projects.st");
            foreach(string s in Rawprojects)
            {
                string ss = s.Replace("\\", "/");
                Project p = new Project(ss, ss.Split('/')[ss.Split('/').Length - 1]);
                if (!isProjectValid(p))
                    continue;
                ListBoxItem item = new ListBoxItem();
                item.Content = p.name + "\n" + p.path;
                item.FontSize = 18;
                item.MouseDoubleClick += onListBoxItemDoubleClick;
                ProjectListBox.Items.Add(item);
                projects.Add(p);
            }
        }
        void addProject(string file)
        {
            if (file == null)
                return;
            string ss = file.Replace("\\", "/");
            Project p = new Project(ss, ss.Split('/')[ss.Split('/').Length - 1]);
            if (!isProjectValid(p))
            {
                MessageBox.Show("Not a valid Project", "Project invalid", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            ListBoxItem item = new ListBoxItem();
            item.Content = p.name + "\n" + p.path;
            item.FontSize = 18;
            item.MouseDoubleClick += onListBoxItemDoubleClick;
            ProjectListBox.Items.Add(item);
            projects.Add(p);
            saveFiles();
        }
        #endregion
        #region Menu
        enum Menu
        {
            None,Home, Projects, Engine
        }
        Menu selectedMenu;
        Color colorMenuItemDefault = Color.FromRgb(31, 31, 31);
        Color colorMenuItemSelected = Color.FromRgb(56, 56, 56);
        private void setSelectedMenu(Menu s)
        {
            selectedMenu = s;
            Menu_Home.Background = new SolidColorBrush(colorMenuItemDefault);
            Menu_Projects.Background = new SolidColorBrush(colorMenuItemDefault);
            Menu_Engine.Background = new SolidColorBrush(colorMenuItemDefault);
            group_home.Visibility = Visibility.Hidden;
            group_projects.Visibility = Visibility.Hidden;
            group_engine.Visibility = Visibility.Hidden;
            group_installEngine.Visibility = Visibility.Hidden;
            EngineDownloadInfo.Visibility = Visibility.Hidden;
            group_home.IsEnabled = false;
            group_projects.IsEnabled = false;
            group_engine.IsEnabled = false;
            group_installEngine.IsEnabled = false;
            EngineDownloadInfo.IsEnabled = false;
            switch (s)
            {
                case Menu.Home:
                    Menu_Home.Background = new SolidColorBrush(colorMenuItemSelected);
                    group_home.Visibility = Visibility.Visible;
                    group_home.IsEnabled = true;
                    break;
                case Menu.Projects:
                    Menu_Projects.Background = new SolidColorBrush(colorMenuItemSelected);
                    group_projects.Visibility = Visibility.Visible;
                    group_projects.IsEnabled = true;
                    break;
                case Menu.Engine:
                    Menu_Engine.Background = new SolidColorBrush(colorMenuItemSelected);
                    group_engine.Visibility = Visibility.Visible;
                    group_engine.IsEnabled = true;
                    break;
            }
        }
        #endregion
        #region events
        private void AddProjectButton_Click(object sender, RoutedEventArgs e)
        {
            if (!CommonFileDialog.IsPlatformSupported)
            {
                using (var fbd = new System.Windows.Forms.FolderBrowserDialog())
                {
                    System.Windows.Forms.DialogResult result = fbd.ShowDialog();

                    if (result == System.Windows.Forms.DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
                    {
                        addProject(fbd.SelectedPath);
                    }
                }
            }
            else
            {
                using (var dialog = new CommonOpenFileDialog())
                {
                    dialog.IsFolderPicker = true;
                    CommonFileDialogResult result = dialog.ShowDialog();

                    if (result == CommonFileDialogResult.Ok && !string.IsNullOrWhiteSpace(dialog.FileName))
                    {
                        addProject(dialog.FileName);
                    }
                }
            }
        }
        private void CreateProjectButton_Click(object sender, RoutedEventArgs e)
        {
            string path = null;
            if (!CommonFileDialog.IsPlatformSupported)
            {
                using (var fbd = new System.Windows.Forms.FolderBrowserDialog())
                {
                    System.Windows.Forms.DialogResult result = fbd.ShowDialog();

                    if (result == System.Windows.Forms.DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
                    {
                        path = fbd.SelectedPath;
                    }
                }
            }
            else
            {
                using (var dialog = new CommonOpenFileDialog())
                {
                    dialog.IsFolderPicker = true;
                    CommonFileDialogResult result = dialog.ShowDialog();

                    if (result == CommonFileDialogResult.Ok && !string.IsNullOrWhiteSpace(dialog.FileName))
                    {
                        path = dialog.FileName;
                    }
                }
            }
            if (path == null)
                return;
            Directory.CreateDirectory(path + "/assets/scenes");
            Directory.CreateDirectory(path + "/config");

            if (Download.CheckForInternetConnection())
            {
                Download.DownloadFile("http://engine.stulu.de/download/default/config/editor-panel-config.ini", path + "/config/editor-panel-config.ini");
                Download.DownloadFile("http://engine.stulu.de/download/default/config/editor-style.ini", path + "/config/editor-style.ini");
                Download.DownloadFile("http://engine.stulu.de/download/default/config/personal-colors.ini", path + "/config/personal-colors.ini");
                Download.DownloadFile("http://engine.stulu.de/download/default//assets/scenes/default.scene", path + "/assets/scenes/default.scene");
            }
            addProject(path);
        }
        private void startpage_web_Navigating(object sender, NavigatingCancelEventArgs e) {
            
            if(!e.Uri.Host.Equals("engine.stulu.de")) {
                e.Cancel = true;
                Process.Start(e.Uri.ToString());
            }
        }
        private void Menu_Home_Click(object sender, RoutedEventArgs e)
        {
            setSelectedMenu(Menu.Home);
        }
        private void Menu_Projects_Click(object sender, RoutedEventArgs e)
        {
            setSelectedMenu(Menu.Projects);
        }
        private void Menu_Engine_Click(object sender, RoutedEventArgs e)
        {
            setSelectedMenu(Menu.Engine);
        }
        private void onListBoxItemDoubleClick(object sender, MouseEventArgs e)
        {
            if (EnginesListBox.SelectedIndex < 0)
            {
                MessageBox.Show("Please select an Engine","No Engine",MessageBoxButton.OK, MessageBoxImage.Error);
                setSelectedMenu(Menu.Engine);
                return;
            }
            selectedProject = ProjectListBox.Items.IndexOf(sender);
            ProcessStartInfo info = new ProcessStartInfo();
            info.FileName = engines[EnginesListBox.SelectedIndex].path + "/Editor.exe";
            info.Arguments = "\"" + projects[selectedProject].path + "\"";
            info.WorkingDirectory = engines[EnginesListBox.SelectedIndex].path;
            Process.Start(info);
        }
        private void ProjectListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            selectedProject = ProjectListBox.SelectedIndex;
            ProjectListBox.SelectedItem = null;
            ProjectListBox.SelectedValue = null;
            ProjectListBox.SelectedValuePath = null;
            ProjectListBox.SelectedIndex = -1;
            ProjectListBox.UnselectAll();
        }
        private void EngineListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            selectedEngine = EnginesListBox.SelectedIndex;
            saveFiles();
        }
        private void EngineInstallButton_Click(object sender, RoutedEventArgs e)
        {
            setSelectedMenu(Menu.None);
            group_installEngine.Visibility = Visibility.Visible;
            group_installEngine.IsEnabled = true;
        }
        private void EnginePageInstallButton_Click(object sender, RoutedEventArgs e)
        {
            installEngine();
        }
        private void engineInstallDirectorySelectButton_Click(object sender, RoutedEventArgs e)
        {
            if (!CommonFileDialog.IsPlatformSupported)
            {
                using (var fbd = new System.Windows.Forms.FolderBrowserDialog())
                {
                    System.Windows.Forms.DialogResult result = fbd.ShowDialog();

                    if (result == System.Windows.Forms.DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
                    {
                        engineInstallDirectory.Text = fbd.SelectedPath;
                    }
                }
            }
            else
            {
                using (var dialog = new CommonOpenFileDialog())
                {
                    dialog.IsFolderPicker = true;
                    CommonFileDialogResult result = dialog.ShowDialog();

                    if (result == CommonFileDialogResult.Ok && !string.IsNullOrWhiteSpace(dialog.FileName))
                    {
                        engineInstallDirectory.Text = dialog.FileName;
                    }
                }
            }
            canInstall();
        }
        private void EngineVeriosnInstallVersionListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canInstall();
        }
        private void EnginePlatformInstallListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            canInstall();
        }
        private void engineInstallDirectory_TextChanged(object sender, TextChangedEventArgs e)
        {
            canInstall();
        }
        #endregion

        int selectedEngine = -1;
        int selectedProject = -1;
        void saveFiles()
        {
            string content = "";
            {//engines
                content = "default|" + selectedEngine + "\n";
                foreach(Engine e in engines)
                {
                    content += e.path + "|" + e.version + "|" + e.platform + "\n";
                }
                File.WriteAllText(dataFolderPath + "/engines.st", content);
            }
            {//projects
                content = "";
                foreach (Project p in projects)
                {
                    content += p.path + "\n";
                }
                File.WriteAllText(dataFolderPath + "/projects.st", content);
            }
        }
        bool isEngineValid(Engine e)
        {
            bool valid = false;
            valid |= File.Exists(e.path + "/Editor.exe");
            valid |= File.Exists(e.path + "/discord-rpc.dll");
            valid |= Directory.Exists(e.path + "/Stulu");
            return valid;
        }
        bool isProjectValid(Project e)
        {
            bool valid = false;
            valid |= Directory.Exists(e.path + "/assets");
            valid |= Directory.Exists(e.path + "/config");
            return valid;
        }
    }
}
