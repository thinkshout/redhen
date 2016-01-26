<?php
/**
 * @file
 * EntityReference extensions for Redhen Contacts.
 */

/**
 * RedHenContact selection handler.
 */
class EntityReference_SelectionHandler_Generic_redhen_contact extends EntityReference_SelectionHandler_Generic {
  /**
   * Build an EntityFieldQuery to get referencable entities.
   */
  protected function buildEntityFieldQuery($match = NULL, $match_operator = 'CONTAINS') {
    $query = parent::buildEntityFieldQuery($match, $match_operator);

    // Filtering by first and last name. EFQs do not support OR conditions, so
    // a tag is added, which allows the resulting query to be altered. In that
    // query_alter, all of the conditions are added, so they are not set here.
    // See http://drupal.stackexchange.com/questions/14499/using-or-with-entityfieldquery
    // and https://api.drupal.org/api/drupal/includes!database!database.inc/function/db_or/7
    // and http://www.phase2technology.com/blog/or-queries-with-entityfieldquery/.
    $query->addTag('redhen_contact_generic_selection');
    // Add a 'search_string' metadata so the query_alter can easily find the
    // search text.
    $query->addMetaData('search_string', $match);

    return $query;
  }

}
