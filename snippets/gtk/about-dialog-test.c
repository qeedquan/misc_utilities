#include <stdio.h>
#include <gtk/gtk.h>

static void
cb_show_about(GtkButton *button, GtkWidget *window)
{

	const gchar *auth[] = {
		"One",
		"Eight",
		"Friday",
		"Usulo",
		NULL,
	};

	gtk_show_about_dialog(GTK_WINDOW(window),
	                      "logo", NULL,
	                      "license", "WTFPL",
	                      "version", "1023",
	                      "program-name", "About Dialog Test",
	                      "comments", "No commento",
	                      "website", "Web Portal",
	                      "copyright", "None",
	                      "authors", auth,
	                      NULL);

	(void)button;
}

int
main(int argc, char *argv[])
{
	GtkWidget *window, *button;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 50);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	g_signal_connect(G_OBJECT(window), "destroy",
	                 G_CALLBACK(gtk_main_quit), NULL);
	button = gtk_button_new_with_label("About");
	g_signal_connect(G_OBJECT(button), "clicked",
	                 G_CALLBACK(cb_show_about), window);
	gtk_container_add(GTK_CONTAINER(window), button);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
