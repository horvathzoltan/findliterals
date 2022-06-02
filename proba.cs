
public string FrameButtonTooltip
{
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
    Console.WriteLine($"Hello World {t4} \\");
    Console.WriteLine($"Hello World \"{t5}\" \\");
    Console.WriteLine(@"Hello World ""\");
    Console.WriteLine("Hello World ddff"+
                      "1fefefefeí");
    Console.WriteLine("Hello World ddff"  +
                              "2fefefefe");
    Console.WriteLine("Hello World ddff"  +"3fefefefe");
    Console.WriteLine("Hello World ddff"  +"4fefefefe");

    Console.WriteLine(@"Hello World ddff
                       fefefefe");
    Console.WriteLine($"Hello World{$"eee"+hu}");


}
