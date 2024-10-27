function generateDirectoryListing()
{
	var nbEntries = entries.length;
	entries.sort();
	for (i=0; i < nbEntries; i++)
	{
		document.writeln('<a href="' + entries[i] + '">' + entries[i] + '</a><br />');
	}
}
