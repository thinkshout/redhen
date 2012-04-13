<?php


/**
 * OG selection handler.
 */
class RedHenOrgGroupSelectionHandler extends EntityReference_SelectionHandler_Generic {

  /**
   * Implements EntityReferenceHandler::getInstance().
   */
  public static function getInstance($field, $instance) {
    return new RedHenOrgGroupSelectionHandler($field, $instance);
  }

 /**
   * Override settings form().
   */
  public static function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);
    return $form;
  }

  /**
   * Build an EntityFieldQuery to get referencable entities.
   */
  public function buildEntityFieldQuery($match = NULL, $match_operator = 'CONTAINS') {
    $handler = EntityReference_SelectionHandler_Generic::getInstance($this->field, $this->instance);
    $query = $handler->buildEntityFieldQuery($match, $match_operator);

    // The "node_access" tag causes errors, so we replace it with
    // "entity_field_access" tag instead.
    // @see _node_query_node_access_alter().
    unset($query->tags['node_access']);
    $query->addTag('entity_field_access');
    $query->addTag('redhen_org_group');

    $group_type = $this->field['settings']['target_type'];
    $entity_info = entity_get_info($group_type);

    if (!field_info_field(OG_GROUP_FIELD)) {
      // There are no groups, so falsify query.
      $query->propertyCondition($entity_info['entity keys']['id'], -1, '=');
      return $query;
    }

    // Show only the entities that are active groups.
    $query->fieldCondition(OG_GROUP_FIELD, 'value', 1, '=');

    $user_groups = og_get_groups_by_user(NULL, $group_type);
    $reference_type = $this->field['settings']['handler_settings']['reference_type'];
    // Show the user only the groups they belong to.
    if ($reference_type == 'my_groups') {
      if ($user_groups && !empty($this->instance) && $this->instance['entity_type'] == 'node') {
        // Check if user has "create" permissions on those groups.
        $node_type = $this->instance['bundle'];
        $ids = array();
        foreach ($user_groups as $gid) {
          if (og_user_access($group_type, $gid, "create $node_type content")) {
            $ids[] = $gid;
          }
        }
      }
      else {
        $ids = $user_groups;
      }

      if ($ids) {
        $query->propertyCondition($entity_info['entity keys']['id'], $ids, 'IN');
      }
      else {
        // User doesn't have permission to select any group so falsify this
        // query.
        $query->propertyCondition($entity_info['entity keys']['id'], -1, '=');
      }
    }
    elseif ($reference_type == 'other_groups' && $user_groups) {
      // Show only groups the user doesn't belong to.
      $query->propertyCondition($entity_info['entity keys']['id'], $user_groups, 'NOT IN');
    }

    return $query;
  }

  public function entityFieldQueryAlter(SelectQueryInterface $query) {
    $handler = EntityReference_SelectionHandler_Generic::getInstance($this->field, $this->instance);
    // FIXME: Allow altering, after fixing http://drupal.org/node/1413108
    // $handler->entityFieldQueryAlter($query);
  }
}
