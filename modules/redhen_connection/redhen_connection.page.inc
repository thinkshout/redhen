<?php

/**
 * @file
 * Contains redhen_connection.page.inc..
 *
 * Page callback for Connection entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Connection templates.
 *
 * Default template: redhen_connection.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_redhen_connection(array &$variables) {
  // Fetch Connection Entity Object.
  $redhen_connection = $variables['elements']['#redhen_connection'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}

/**
* Prepares variables for a custom entity type creation list templates.
*
* Default template: redhen_connection-content-add-list.html.twig.
*
* @param array $variables
*   An associative array containing:
*   - content: An array of redhen_connection-types.
*
* @see block_content_add_page()
*/
function template_preprocess_redhen_connection_content_add_list(&$variables) {
  $variables['types'] = array();
  $entity = $variables['entity'];
  // @todo abstract deriving the {redhen_type} param from the entity itself.
  $entity_type = explode('_', $entity->getEntityType()->id());
  $redhen_type = array_pop($entity_type);
  $query = \Drupal::request()->query->all();
  foreach ($variables['content'] as $type) {
    $variables['types'][$type->id()] = array(
      'link' => Link::fromTextAndUrl($type->label(), new Url('entity.redhen_connection.add_form', array(
        'redhen_type' => $redhen_type,
        'entity' => $entity->id(),
        'redhen_connection_type' => $type->id(),
      ), array('query' => $query))),
      'description' => array(
      '#markup' => $type->label(),
      ),
      'title' => $type->label(),
      'localized_options' => array(
      'query' => $query,
      ),
    );
  }
}
