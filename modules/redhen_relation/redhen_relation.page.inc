<?php

/**
 * @file
 * Contains redhen_relation.page.inc..
 *
 * Page callback for Relation entities.
 */

use Drupal\Core\Render\Element;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Prepares variables for Relation templates.
 *
 * Default template: redhen_relation.html.twig.
 *
 * @param array $variables
 *   An associative array containing:
 *   - elements: An associative array containing the user information and any
 *   - attributes: HTML attributes for the containing element.
 */
function template_preprocess_redhen_relation(array &$variables) {
  // Fetch Relation Entity Object.
  $redhen_relation = $variables['elements']['#redhen_relation'];

  // Helpful $content variable for templates.
  foreach (Element::children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }
}

/**
* Prepares variables for a custom entity type creation list templates.
*
* Default template: redhen_relation-content-add-list.html.twig.
*
* @param array $variables
*   An associative array containing:
*   - content: An array of redhen_relation-types.
*
* @see block_content_add_page()
*/
function template_preprocess_redhen_relation_content_add_list(&$variables) {
  $variables['types'] = array();
  $query = \Drupal::request()->query->all();
  foreach ($variables['content'] as $type) {
    $variables['types'][$type->id()] = array(
      'link' => Link::fromTextAndUrl($type->label(), new Url('entity.redhen_relation.add_form', array(
        'redhen_relation_type' => $type->id()
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
