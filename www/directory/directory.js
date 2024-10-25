//var images = ["1", "2", "3"];
//var prefix = "Reflections_"
//var prefixMini = "thumb/Reflections_tb"
//var extFile = ".jpg";
//var extMini = ".jpg";

//nbimages = 10;

//for (i in images)

var entries = ["1", "2", "3"];

generateDirectoryListing()
{
	var nbEntries = entries.length;
	for (i=1; i<=nbEntries ;i++)
	{
		//image = prefix+images[i];
		num = i;
		if (num < 10) num = "0" + num;
		image = prefix + num;
		miniPath = prefixMini + num + extMini;
		document.writeln('<a href="'+image+extFile+'"><img src="'+miniPath+'" alt="'+image+'" title="'+image+'" /></a>');
	}
}
