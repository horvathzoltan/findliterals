
public static string GetInsoleRFileName(Guid e, PhysDirection dir)
      {
          return GetInsoleFolder(e, dir)+"\\R.txt";
      }

public static string GetInsoleDataFileName(Guid e, PhysDirection dir)
      {
          return GetInsoleFolder(e, dir) + "\\data.txt";
      }

public static string GetInsoleStatsFileName(Guid e, PhysDirection dir)
      {
          return GetInsoleFolder(e, dir) + "\\stats.txt";
      }

public static string GetCameraStatsFileName(Guid e, PhysDirection dir)
      {
          return GetCameraFolder(e, dir) + "\\stats.txt";
      }

public string FrameButtonTooltip
{
    string e = "";
    get
    {
        FrameButtonTooltip_ = GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.FrameButtonTooltip");
        switch (RectangleStatus)
        {
            case RectangleStatusEnum.Undefined:
                FrameButtonTooltip_ += $"\n{GlobalTranslation.Translate("WC_UI_UWP_Pages.{CalibrationItemControl}.RectangleStatusEnum_Current")}{GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.RectangleStatusEnum_Undefined")}"/*nowc*/;
                break;
            case RectangleStatusEnum.Processing:
                FrameButtonTooltip_ += $"\n{GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.RectangleStatusEnum_Current")}{GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.RectangleStatusEnum_Processing")}"/*nowc*/;"makmak"
                break;
            case RectangleStatusEnum.Defined:
                FrameButtonTooltip_ += $"\n{GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.RectangleStatusEnum_Current")}{GlobalTranslation.Translate("WC_UI_UWP_Pages.CalibrationItemControl.RectangleStatusEnum_Defined")}";/*nowc*/
                break;
        }
        return FrameButtonTooltip_;
    }
    set
    {

    }
}


private static string CreateToString<T>(List<PropertyInfo> properties, T instance) where T : class
     {
         var type = typeof(T);
         if (instance == null)
         {
             return type.Name;
         }

         StringBuilder strB = new StringBuilder();
         strB.Append($"{type.Name}: ");
         properties.ForEach(p =>
         {
             var value = p.GetValue(instance);
             strB.Append($"{p.Name}: {(value == null ? "null" : value.ToString())} --- ");
         });
         return strB.ToString().TrimEnd('-', ' ');
     }

/*
Hello World 7 \
Hello World "7" \
Hello World "\
*/

// "huhu"
private static void test2()
{
    int t4=7;
    int t5=8;
    string hu = "hu-HU";

    Console.WriteLine($"Hello World {t4} \\");
    Console.WriteLine($"Hello World \"{t5}\" \\");
    Console.WriteLine(@"555Hello World ""\");
    Console.WriteLine("Hello World ddff"+
                      "1fefefefeí");
    Console.WriteLine("Hello World ddff"  +
                              "2fefefefe");
    Console.WriteLine("Hello World ddff"  +"3fefefefe");
    Console.WriteLine("Hello World ddff"  +"4fefefefe");

    Console.WriteLine(@"Hello World ddff
                       fefefefe");
    Console.WriteLine($"Hello World{$"eee"+hu}");

    Console.WriteLine("Hello World {t6} \\");
    Console.WriteLine("Hello World \"{t6}\" \\");
}

using System;
using System.Collections.Generic;
using System.Text;

namespace MasterMove.InsoleLib
{
    public static class StringHelper2
    {
        public static IEnumerable<int> AllIndexesOf(this string str, string searchstring)
        {
            int minIndex = str.IndexOf(searchstring);
            while (minIndex != -1)
            {
                yield return minIndex;
                minIndex = str.IndexOf(searchstring, minIndex + searchstring.Length);
            }
        }

        private static Dictionary<char, char> cctable = new Dictionary<char, char>() {
            {'á','a'},
            {'é','e'},
            {'í','i'},
            {'ó','o'},{ 'ö','o'},{ 'ő','o'},
            {'ú','u'},{ 'ü','u'},{ 'ű','u'},

            {'Á','a'},
            {'É','e'},
            {'Í','i'},
            {'Ó','o'},{ 'Ö','o'},{ 'Ő','o'},
            {'Ú','u'},{ 'Ü','u'},{ 'Ű','u'},

            {'A','a'},
            {'B','b'},
            {'C','c'},
            {'D','d'},
            {'E','e'},
            {'F','f'},
            {'G','g'},
            {'H','h'},
            {'I','i'},
            {'J','j'},
            {'K','k'},
            {'L','l'},
            {'M','m'},
            {'N','n'},
            {'O','o'},
            {'P','p'},
            {'Q','q'},
            {'R','r'},
            {'S','s'},
            {'T','t'},
            {'U','u'},
            {'V','v'},
            {'W','w'},
            {'Z','z'},
            {'X','x'},
            {'Y','y'}
        };

        public static string zNormalize(this string s)
        {
            char c;
            string e = "";
            foreach (char sc in s)
                if (!Char.IsWhiteSpace(sc))
                {
                    if (cctable.TryGetValue(sc, out c))
                        e += c;
                    else
                        e += sc;
                }
            return e;
        }

        //public static string Append(this string m, string s)
        //{
        //    if (!string.IsNullOrEmpty(m)) m += ", ";
        //    m += s;
        //    return m;
        //}

        //public static void AppendNL(ref string m, params object[] s)
        //{
        //    if (!string.IsNullOrEmpty(m) && !m.EndsWith(Environment.NewLine)) m += Environment.NewLine;
        //    m += String.Join("", s);
        //}

        //private static readonly CultureInfo ENCU = new CultureInfo("en-US");
        //public static float ToFloat(string s)
        //{
        //    if (string.IsNullOrEmpty(s)) return 0;
        //    try
        //    {
        //        var e = Convert.ToSingle(s.Replace(',', '.'), ENCU);
        //        return e;
        //    }
        //    catch (Exception ex)
        //    {
        //        return 0;
        //    }
        //}

        //public static double ToDouble(string s)
        //{
        //    if (string.IsNullOrEmpty(s)) return 0;
        //    try
        //    {
        //        var e = Convert.ToDouble(s.Replace(',', '.'), ENCU);
        //        return e;
        //    }
        //    catch (Exception ex)
        //    {
        //        return 0;
        //    }
        //}

        //public static bool ToBool(string response)
        //{
        //    if (string.IsNullOrEmpty(response)) return false;
        //    string s = response.ToLower();
        //    return s == "1" || s == tr(tre.STRIN1) || s == "ok";
        //}
        /**/
    }
}
