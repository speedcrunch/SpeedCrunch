INSTALL(FILES       books.css
        DESTINATION ${BOOKSDIR}
)

INSTALL(DIRECTORY   images en fi pt
        DESTINATION "${BOOKSDIR}"
        PATTERN     ".svn" EXCLUDE
)
