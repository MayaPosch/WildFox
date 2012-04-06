
#ifndef TYPES_H
#define TYPES_H


struct Filter {
    int extId;                  // ID of the extension page object, if any.
    QString scheme;             // Used scheme (HTTP or HTTPS)
    QList<QString> segments;    // URL string split on dots.
    QList<QString> scripts;     // paths to the scripts to be included in source.
};


#endif // TYPES_H
