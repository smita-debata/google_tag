<?php

/**
 * @file
 * Contains the JavaScript snippet insertion code.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Returns JavaScript snippet.
 *
 * @return string
 *   The JavaScript snippet.
 *
 * @see google_tag_page_build()
 */
function google_tag_snippet() {
  // Gather data.
  $container_id = variable_get('google_tag_container_id', '');
  $container_id = trim(drupal_json_encode($container_id), '"');
  $data_layer = variable_get('google_tag_data_layer', 'dataLayer');
  $data_layer = trim(drupal_json_encode($data_layer), '"');
  $compact = variable_get('google_tag_compact_tag', 1);

  // Build script tags.
  $noscript = <<<EOS
<noscript><iframe src="//www.googletagmanager.com/ns.html?id=$container_id"
 height="0" width="0" style="display:none;visibility:hidden"></iframe></noscript>
EOS;
  $script = <<<EOS
<script type="text/javascript">
(function(w,d,s,l,i){

  w[l]=w[l]||[];
  w[l].push({'gtm.start':new Date().getTime(),event:'gtm.js'});
  var f=d.getElementsByTagName(s)[0];
  var j=d.createElement(s);
  var dl=l!='dataLayer'?'&l='+l:'';
  j.src='//www.googletagmanager.com/gtm.js?id='+i+dl;
  j.type='text/javascript';
  j.async=true;
  f.parentNode.insertBefore(j,f);

})(window,document,'script','$data_layer','$container_id');
</script>
EOS;
  if ($compact) {
    $noscript = str_replace("\n", '', $noscript);
    $script = str_replace(array("\n", '  '), '', $script);
  }
  $script = <<<EOS

<!-- Google Tag Manager -->
$noscript
$script
<!-- End Google Tag Manager -->
EOS;

  // Allow other modules to alter the script.
  drupal_alter('google_tag_snippet', $script);
  return $script;
}
